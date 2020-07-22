#include "chatServerEngine.h"
#include "net_func_wrappers.h"
#include <sstream>
#include <string>
#include <thread>
#include <algorithm>

using namespace thor;

namespace
{

   void printSafe(const std::string& message,int fd = -1)
   {
      std::mutex mutex;
      std::lock_guard<std::mutex> guardStandarOutput(mutex);
      if( fd != -1)
         std::cout<< message << fd << std::endl;
      else
         std::cout << message << std::endl;
   }
}

ChatServerEngine::~ChatServerEngine()
{
    shutdown(m_listenSocket,SHUT_RDWR);
}

void ChatServerEngine::initializeServer( int portNumber,const char* ipAddress )
{
    installSignalHandlers();
    m_listenSocket = createServerSocket(portNumber,ipAddress);
}

bool ChatServerEngine::run()
{
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
      int client_fd = Accept(m_listenSocket,(sockaddr*)&client_address,&len);
      
    
   }

}

void ChatServerEngine::startNewServiceThread()
{ 
    std::thread serviceThread(&ChatServerEngine::serviceClient,this);
    serviceThread.detach();
}

void ChatServerEngine::installSignalHandlers()
{
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
}

int ChatServerEngine::createServerSocket(int portNumber,const char* serverIpAddress )
{
     //create listening socket file descriptor
    int listen_fd = Socket(AF_INET, SOCK_STREAM, 0);
  
    //server address
    sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    if( serverIpAddress)
        inet_pton(AF_INET,serverIpAddress,&server_address.sin_addr);
    else
        server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(portNumber);
   
   // bind server socket
   Bind(listen_fd, (struct sockaddr*)&server_address, sizeof(server_address));

   std::cout << "listening on:"<< inet_ntoa(server_address.sin_addr) << ": "<< ntohs(server_address.sin_port) << std::endl;

   //Listen
   Listen(listen_fd,5);

   return listen_fd;
}

void ChatServerEngine::addNewClient( const sockaddr_in& client_address, int clientSocketFd)
{
    char buffer[40];
    ClientInfo newClient;
   
   // newClient.name = clientName;
    newClient.socketFd = clientSocketFd;
    newClient.port = ntohs(client_address.sin_port);
    newClient.address = inet_ntop(AF_INET,&client_address.sin_addr, buffer,sizeof(buffer));

    std::stringstream oss;
    oss << "conecction from: "<< newClient.address<< ", port: "<< newClient.port<<std::endl;
    printSafe(oss.str());

    // add the new client to the list for other clients 
    int numClients{};
    {
      std::lock_guard<std::mutex> lockguard(m_mutex);
      m_clientsList.push_back(newClient);
      numClients = m_clientsList.size();
    }

    printSafe("num clients: ",numClients);

}

//this is the service thread where messages are handle for different clients
void ChatServerEngine::serviceClient()
{
   ClientInfo thisClient;
   {
      std::lock_guard<std::mutex> lockthis(m_mutex);
      thisClient = *(--m_clientsList.end());
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
         int errnocode{};
         {
            std::lock_guard<std::mutex> lockthis(m_mutex);
            errnocode = errno;
         }
            if ( errnocode != EINTR )// chek if the read system call was interrupted
               //we have an error
               err_sys("fatal error reciving data");
         
      }
      else if ( bytesReceived == 0 || message == "exit" )
      {
         connected = false;
      }
      else
      {
         //send the message
         sendMsgToClients(thisClient.socketFd,message);
         memset(buffer,0,sizeof(buffer));
      }
   }
     
   //Remove the client from the list if it was not previously removed
   findAndRemoveClient(thisClient);
   printSafe("closing client:",thisClient.socketFd);
   //close socket
   thor::Close(thisClient.socketFd);
   sleep(1);
}

void ChatServerEngine::sendMsgToClients(int senderFd, const std::string& message)
{
   // lock the vector
   std::lock_guard<std::mutex> guard_list(m_mutex);
   //send the data from the client to others
   for ( std::size_t i = 0; i < m_clientsList.size();++i)
   {
      ClientInfo& currClient = m_clientsList[i];
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

void ChatServerEngine::findAndRemoveClient(const ClientInfo& clientToClose)
{
   std::lock_guard<std::mutex> guardClientList(m_mutex);
   auto listBegIt = m_clientsList.begin();
   auto listEndIt = m_clientsList.end();
   auto clientToRemove = std::find(listBegIt,listEndIt,clientToClose);

   if( clientToRemove != listEndIt)
   {
      m_clientsList.erase(clientToRemove);
      std::cout << "client Removed from list:"<<  clientToClose.name<<std::endl;;
   }
   
}

void ChatServerEngine::sendMsgTo(int clientSocketFd,const std::string& message)
{
    //Sendata
   int bytes = send(clientSocketFd,message.c_str(),message.size(),0);
   printSafe( "bytes sent",bytes);
}