#include "chatServerEngine.h"
#include "message.h"
#include "JsonFormatter.h"
#include "net_func_wrappers.h"
#include "DataBaseAcces.h"
#include <sstream>
#include <string>
#include <thread>
#include <algorithm>

using namespace thor;

namespace
{
   std::atomic<bool>serverClosed{false};
   
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

ChatServerEngine::ChatServerEngine()
   : m_clientsList{}
   , m_listenSocket{}
   , m_clientsMutex{}
   ,m_messageMutex{}
   ,m_ptrSenderThread{nullptr}
{
}

ChatServerEngine::~ChatServerEngine()
{
   if( m_ptrSenderThread->joinable())
      m_ptrSenderThread->join();
   shutdown(m_listenSocket,SHUT_RDWR);
}

void ChatServerEngine::initializeServer( int portNumber,const char* ipAddress )
{
   installSignalHandlers();
   m_listenSocket = createServerSocket(portNumber,ipAddress);
   m_ptrSenderThread.reset( new std::thread{&ChatServerEngine::sendServiceThreadFunc,this});
}

bool ChatServerEngine::run()
{
   // client information
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
      printSafe("Client connected...");
      std::thread loginServiceThread(&ChatServerEngine::loginServiceThreadFunc,this,client_fd,client_address);
      loginServiceThread.detach();
   }

}

void ChatServerEngine::loginServiceThreadFunc(int socketClientFd, sockaddr_in clientAddr)
{
   bool loginSucceed{false};
   char buffer[40];
   ClientInfo newClient;
   JsonFormatter formatter;
   DataBaseAccess dataBaseAccess("users.db");
   dataBaseAccess.logInDB();

   while( !serverClosed && !loginSucceed)
   {
      std::string initMessage = readSocket(socketClientFd);

      if( initMessage.empty()) break;

      formatter.parseLoginInfo(newClient,initMessage);
      

      /// check database for user name and password
      std::cout << "name:"<<newClient.name << std::endl;
      std::cout << "password"<< newClient.password << std::endl;
      //retrive the image data and grant access
      auto result = dataBaseAccess.querryString(newClient.name.c_str());  
      
      loginSucceed = !result.empty();
      std::string response;
      if( !loginSucceed)
      {
         // check if it is a sign up message
         // formatter.parseSignInInfo()

         //Send a message repeat the information
         response = formatter.formatData("failed","server",MsgType::LOGIN);
         
      }
      else
      {
         response = formatter.formatData("succeed","server",MsgType::LOGIN);
      }
      
      sendMsgTo(socketClientFd,response);
   }

  if( loginSucceed && !serverClosed)
  {
      newClient.socketFd = socketClientFd;
      newClient.port = ntohs(clientAddr.sin_port);
      newClient.address = inet_ntop(AF_INET,&clientAddr.sin_addr, buffer,sizeof(buffer));

      std::stringstream oss;
      oss << "conecction from: "<< newClient.address<< ", port: "<< newClient.port<<std::endl;
      printSafe(oss.str());
      //if loggin succesfull
      addNewClient(newClient);

      //launch its own reading thread
      std::thread clientReadThread {&ChatServerEngine::readServiceThreadFunc,this,newClient};
      clientReadThread.detach();
  }

}

std::string ChatServerEngine::readSocket(int socket)
{
      //message buffer from the readed socket
      char buffer[MAXLINE];
      memset(buffer,0,sizeof(buffer));

      //read if there is data from the client
      int bytesReceived = recv(socket, buffer, sizeof(buffer), 0);
      
      if( bytesReceived < 0 )
      {
         int errnocode{};
         {
            
            std::lock_guard<std::mutex> lockthis(m_clientsMutex);
            errnocode = errno;
         }
         if ( errnocode != EINTR )// chek if the read system call was interrupted
            //we have an error
            err_sys("fatal error reciving data");
      }
       else if ( bytesReceived == 0)
      {
         // client left somehow we just break;
         return std::string();
      }
     
     return std::string(buffer);
      
}



 //read service this is unique for each client
void ChatServerEngine::readServiceThreadFunc(ClientInfo thisClient)
{
   //handle client
   printSafe("start transaction:",thisClient.socketFd);

   while(!serverClosed)
   {
     //read if there is data from the client
      std::string message = readSocket(thisClient.socketFd);
      
      if ( message.empty()) break;

      //Add message to the message queue
      addMessageToQueue(thisClient.socketFd,message);
   }
   
   findAndRemoveClient(thisClient);
   printSafe("closing client:",thisClient.socketFd);
   //close socket
   thor::Close(thisClient.socketFd);
   sleep(1);
}


void ChatServerEngine::addMessageToQueue(int clientSocket,const std::string& message)
{
   {
      std::lock_guard<std::mutex> lockMessageQueue(m_messageMutex);
      messageQueue.push_back(std::pair<int,std::string>(clientSocket,message));
   }
}
// Send service just forward the messages in the message queue to all the clients
void ChatServerEngine::sendServiceThreadFunc()
{
   
   while(!serverClosed)
   {
      { //reduce the scope of the lock
         std::unique_lock<std::mutex> guard1(m_messageMutex, std::defer_lock);
         std::unique_lock<std::mutex> guard2(m_clientsMutex, std::defer_lock);
         std::lock(guard1, guard2);
         for( auto& msg : messageQueue)
         {
            
            int senderFd = msg.first;
            std::string message = msg.second;
            //send the data from the client to others
            for ( std::size_t i = 0; i < m_clientsList.size();++i)
            {
               ClientInfo& currClient = m_clientsList[i];
               auto receiverFd = currClient.socketFd;
               bool clientIsConnected =  fcntl(receiverFd, F_GETFD) != -1 || errno != EBADF;
               
               if( receiverFd != senderFd)
               {
                     
                  if ( clientIsConnected )
                  {
                     printSafe("----------------------");
                     printSafe(std::string("send: ")+message,receiverFd);   
                     printSafe("----------------------");
                     send(receiverFd,message.c_str(),message.size(),0);//Sendata
                  }
               }
            }
         } 
         messageQueue.clear();
      }
   }

   

}

void ChatServerEngine::installSignalHandlers()
{
     // ctr+c signal
   setSignalHandler(SIGINT, [](int signo)
   {
      //notify other threads to finish
      serverClosed = true;
      //just exit this will close all the sockets open
      exit(0);
   });
   // ctr+c signal
   setSignalHandler(SIGTERM, [](int signo)
   {
      //notify other threads to finish
      serverClosed = true;
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

void ChatServerEngine::addNewClient( const ClientInfo& newClient)
{
    // add the new client to the list for other clients 
    int numClients{};
    {
      std::lock_guard<std::mutex> lockguard(m_clientsMutex);
      m_clientsList.push_back(newClient);
      numClients = m_clientsList.size();
    }

    printSafe("num clients: ",numClients);

}


void ChatServerEngine::findAndRemoveClient(const ClientInfo& clientToClose)
{
   std::lock_guard<std::mutex> guardClientList(m_clientsMutex);
   auto listBegIt = m_clientsList.begin();
   auto listEndIt = m_clientsList.end();
   auto clientToRemove = std::find(listBegIt,listEndIt,clientToClose);

   if( clientToRemove != listEndIt)
   {
      m_clientsList.erase(clientToRemove);
      std::cout << "client Removed from list:"<<  clientToClose.name<<std::endl;
   }
   
}

void ChatServerEngine::sendMsgTo(int clientSocketFd,const std::string& message)
{
    //Sendata
   int bytes = send(clientSocketFd,message.c_str(),message.size(),0);
   printSafe( "bytes sent",bytes);
}