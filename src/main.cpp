#include "net_func_wrappers.h"
#include <time.h>
#include <map>
#include <vector>
#include <thread>
#include <algorithm>
#include <functional>
#include <mutex>
#include <sstream>


std::mutex mutex;

void printSafe(const std::string& message,int fd = -1)
{
   std::lock_guard<std::mutex> guardStandarOutput(mutex);
   if( fd != -1)
      std::cout<< message << fd << std::endl;
   else
      std::cout << message << std::endl;
}
template <typename T>
void findAndRemoveClient(std::vector<T>& clientsList, int clientFileDescriptor)
{
   std::lock_guard<std::mutex> guardClientList(mutex);
   auto listBegIt = clientsList.begin();
   auto listEndIt = clientsList.end();
   auto clientToRemove = std::find(listBegIt,listEndIt,clientFileDescriptor);

   if( clientToRemove != listEndIt)
   {
      clientsList.erase(clientToRemove);
      std::cout << "client Removed from list:"<<  clientFileDescriptor<<std::endl;;
   }
   
}

template <typename T>
void removeClientByIndex(std::vector<T>& clientsList, int index)
{
   auto itPosition = clientsList.begin();
   std::advance(itPosition,index);
   clientsList.erase(itPosition);
}

void sendMsg(int senderFd, const std::string& message, std::vector<int>& clientsList)
{
   // lock the vector
   std::lock_guard<std::mutex> guard_list(mutex);
   //send the data from the client to others
   for ( int i = 0; i < clientsList.size();++i)
   {
      int& currClient = clientsList[i];
      bool clientIsConnected =  fcntl(currClient, F_GETFD) != -1 || errno != EBADF;
   
      if( currClient != senderFd)
      {
         if ( clientIsConnected )
            //Sendata
            send(currClient,message.c_str(),message.size(),0);
      }
   }
}

void serviceClient(int fd,std::vector<int>& clientsList)
{
   //Send greetings message back to the client 
   char greetings[MAXLINE];
   memset(greetings,0,sizeof(greetings));
   snprintf(greetings, sizeof(greetings), "Welcome to the chat server introduce your name\r\n");
   send(fd,greetings, strlen(greetings),0);
     
   //handle client
   bool connected{true};
   char buffer[MAXLINE];
     
   while(connected)
   {
      //read if there is data from the client
      int bytesReceived = recv(fd, buffer, sizeof(buffer), 0);

      printSafe("Client :",fd);
      printSafe("bytes received: ", bytesReceived);   
      printSafe("----------------------");
      std::string message{buffer};
         
      if( bytesReceived < 0 )
      {
         {
            std::lock_guard<std::mutex> lockthis(mutex);
            if ( errno != EINTR )// chek if the read system call was interrupted
               //we have an error
               err_sys("fatal error reciving data");

         }
      }
      else if ( bytesReceived == 0 || message == "exit" )
      {
         break;
      }
      else
      {
         //send the message
         sendMsg(fd,message,clientsList);
         memset(buffer,0,sizeof(buffer));
      }
   }
     
     thor::Close(fd);
     //Remove the client from the list if it was not previously removed
     findAndRemoveClient(clientsList, fd);
     printSafe("closing client:",fd);
     sleep(1);
     //close socket
}

int CreateServerSocket()
{

   //create listening socket file descriptor
    int listen_fd = Socket(AF_INET, SOCK_STREAM, 0);
  
    //server address
    sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(SERVER_PORT);
   
   // bind server socket
   Bind(listen_fd, (struct sockaddr*)&server_address, sizeof(server_address));

   std::cout << "listening on:"<< inet_ntoa(server_address.sin_addr) << ": "<< ntohs(server_address.sin_port) << std::endl;

   //Listen
   Listen(listen_fd,5);

   return listen_fd;
}

int main()
{
   std::vector<int> clientsList;

   // ctr+c signal
   setSignalHandler(SIGINT, [](int signo)
   {
      //just exit this will close all the sockets open
      exit(0);
   });
   //server file descriptor or socket
   int listen_fd = CreateServerSocket();

   // client file descriptor
   sockaddr_in client_address;

   //buffer for the message
   char buffer[MAXLINE];
 
   while(true)
   {
      socklen_t len = sizeof(client_address);
      printSafe("Listening for clients...");
      //Accept client
      int client_fd = Accept(listen_fd,(sockaddr*)&client_address,&len);
      std::stringstream oss;
      oss << "conecction from: "<< inet_ntop(AF_INET, &client_address.sin_addr,buffer, sizeof(buffer))<< ", port: "<< ntohs(client_address.sin_port)<<std::endl;
      printSafe(oss.str());
      mutex.lock();
      // add the new client to the list for other clients 
      clientsList.push_back(client_fd);
      int numClients = clientsList.size();
      mutex.unlock();
      printSafe("num clients: ",numClients);
      //start the service thread
      std::thread serviceThread(serviceClient,client_fd,std::ref(clientsList));
      serviceThread.detach();
   }
 
return 0;
}
