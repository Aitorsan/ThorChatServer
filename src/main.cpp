#include "net_func_wrappers.h"
#include "ClientInfo.h"
#include "json11.hpp"
#include "base64.h"
#include <time.h>
#include <map>
#include <vector>
#include <thread>
#include <algorithm>
#include <functional>
#include <mutex>
#include <sstream>
#include <fstream>


std::mutex mutex;

void printSafe(const std::string& message,int fd = -1)
{
   std::lock_guard<std::mutex> guardStandarOutput(mutex);
   if( fd != -1)
      std::cout<< message << fd << std::endl;
   else
      std::cout << message << std::endl;
}


void findAndRemoveClient(std::vector<ClientInfo>& clientsList, const ClientInfo& clientToClose)
{
   std::lock_guard<std::mutex> guardClientList(mutex);
   auto listBegIt = clientsList.begin();
   auto listEndIt = clientsList.end();
   auto clientToRemove = std::find(listBegIt,listEndIt,clientToClose);

   if( clientToRemove != listEndIt)
   {
      clientsList.erase(clientToRemove);
      std::cout << "client Removed from list:"<<  clientToClose.name<<std::endl;;
   }
   
}

template <typename T>
void removeClientByIndex(std::vector<T>& clientsList, int index)
{
   auto itPosition = clientsList.begin();
   std::advance(itPosition,index);
   clientsList.erase(itPosition);
}

void sendMsg(int senderFd, const std::string& message, std::vector<ClientInfo>& clientsList)
{
   // lock the vector
   std::lock_guard<std::mutex> guard_list(mutex);
   //send the data from the client to others
   for ( int i = 0; i < clientsList.size();++i)
   {
      ClientInfo& currClient = clientsList[i];
      auto receiverFd = currClient.socketFd;
      bool clientIsConnected =  fcntl(receiverFd, F_GETFD) != -1 || errno != EBADF;
   
      if( receiverFd != senderFd)
      {
         if ( clientIsConnected )
            //Sendata
            send(receiverFd,message.c_str(),message.size(),0);
      }
   }
}

void sendMsgTo(int clientSocketFd,const std::string& message)
{
    //Sendata
   int bytes = send(clientSocketFd,message.c_str(),message.size(),0);
   printSafe( "bytes sent",bytes);
}

void serviceClient(std::vector<ClientInfo>& clientsList)
{
   ClientInfo thisClient;
   {
      std::lock_guard<std::mutex> lockthis(mutex);
      thisClient = *(--clientsList.end());
   }
  
   //handle client
   bool connected{true};
   char buffer[MAXLINE];
   memset(buffer,0,sizeof(buffer));
printSafe("start transaction:",thisClient.socketFd);
   while(connected)
   {
      //read if there is data from the client
      int bytesReceived = recv(thisClient.socketFd, buffer, sizeof(buffer), 0);

      printSafe("----------------------");
      printSafe("Client :",thisClient.socketFd);
      printSafe("bytes received: ", bytesReceived);   
      printSafe("----------------------");
      std::string message{"\n["+thisClient.name + "]:"+buffer+'\n'};
         
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
         connected = false;
      }
      else
      {
         //send the message
         sendMsg(thisClient.socketFd,message,clientsList);
         memset(buffer,0,sizeof(buffer));
      }
   }
     
     //Remove the client from the list if it was not previously removed
     findAndRemoveClient(clientsList, thisClient);
     printSafe("closing client:",thisClient.socketFd);
     //close socket
     thor::Close(thisClient.socketFd);
     sleep(1);
}



std::string ReadImage()
{
   std::stringstream os;
   os << std::ifstream("/home/aitor/Desktop/Projects/c++/ChatServer/images/im.jpg", std::ios::binary).rdbuf();
   return os.str();
    
}

void SaveFile(std::string imageData)
{
   std::istringstream buffer(imageData);
   std::ofstream("stuff.json") << buffer.rdbuf();

}


std::string createImagePacket(const std::string& fileName,const std::string& imageTypeExtension, std::string& data)
{
   std::vector<std::pair<std::string,std::string> >jsonData = {
    { "\"type\"", "\"image\",\n" },
    { "\"filename\"", std::string("\"")+fileName+std::string("\",\n") },
     {"\"extension\"",std::string("\"")+imageTypeExtension+std::string("\",\n")}, /* .jpg , .png*/
    { "\"data\"", std::string("\"")+base64(data) +std::string("\"")},
   };

   std::string json_str{"{\n"};
   for ( auto all : jsonData)
   {
      json_str += all.first;
      json_str+= ":";
      json_str+= all.second;
  
   }
   
   json_str += "\n}\n";
   return json_str;
}


std::string createImagePacketwithJson11(const std::string& fileName,const std::string& imageTypeExtension,std::string& data)
{
  std::string database64 = base64(data);
   SaveFile(database64);
   json11::Json my_json = json11::Json::object {
    { "type", "image" },
    {"filename",fileName},
    { "extension", imageTypeExtension },
    { "data", database64 }
};
   std::string json_str = my_json.dump();
   
   return json_str;
  
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

   std::string image = ReadImage();
   std::string jsonPacket = createImagePacket("somefile","jpg",image);
   std::vector<ClientInfo> clientsList;
   SaveFile(jsonPacket);
    std::cout << jsonPacket << std::endl;
   // ctr+c signal
   setSignalHandler(SIGINT, [](int signo)
   {
      //just exit this will close all the sockets open
      exit(0);
   });
   // ctr+c signal
   setSignalHandler(SIGTERM, [](int signo)
   {
      //just exit this will close all the sockets open
      exit(0);
   });
   //server file descriptor or socket
   int listen_fd = CreateServerSocket();

   // client file descriptor
   sockaddr_in client_address;
 
   //buffer for the address
   char buffer[20];
   while(true)
   {
      memset(buffer,0,sizeof(buffer));
      socklen_t len = sizeof(client_address);
      printSafe("Listening for clients...");
      //Accept client
      int client_fd = Accept(listen_fd,(sockaddr*)&client_address,&len);

      std::string clientName {"whatever"};
      /*int numBytes = recv(client_fd, buffer,sizeof(buffer),0);
      if( numBytes < 0 )
      {
         if ( errno != EINTR )// chek if the read system call was interrupted
            //we have an error
            err_sys("fatal error reciving data");
      }
      else
      {
       //Send greetings message back to the client 
       */
      sendMsgTo(client_fd, jsonPacket);
      //}
      sleep(0.5);
     
      //TODO: find a way to serialize the client information
      ClientInfo newClient;
      newClient.name = clientName;
      newClient.socketFd = client_fd;
      newClient.port = ntohs(client_address.sin_port);
      newClient.address = inet_ntop(AF_INET,&client_address.sin_addr, buffer,sizeof(buffer));

      std::stringstream oss;
      oss << "conecction from: "<< newClient.address<< ", port: "<< newClient.port<<std::endl;
      printSafe(oss.str());
      // add the new client to the list for other clients 
      mutex.lock();
      clientsList.push_back(newClient);
      int numClients = clientsList.size();
      mutex.unlock();
      
      printSafe("num clients: ",numClients);
      //start the service thread
      std::thread serviceThread(serviceClient,std::ref(clientsList));
      serviceThread.detach();
   }
 shutdown(listen_fd,SHUT_RDWR);
return 0;
}
