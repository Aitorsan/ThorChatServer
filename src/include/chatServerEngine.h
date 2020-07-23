#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include "ClientInfo.h"
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <memory>
#include <utility>
#include <netinet/in.h>
class ChatServerEngine
{
 public:
    ChatServerEngine();
    ~ChatServerEngine();

    void initializeServer( int portNumber,const char* ipAddress = nullptr);
    bool run();

private:
    void installSignalHandlers();
    int createServerSocket( int portNumber,const char* serverAddress);
    void addNewClient( const ClientInfo& newClient);
    void findAndRemoveClient(const ClientInfo& clientToClose);
    void sendMsgTo(int clientSocketFd,const std::string& message);
    void addMessageToQueue(int clientSocket,const std::string& message);
    std::string readSocket(int socket);

    //threaded services
    void readServiceThreadFunc(ClientInfo thisClient);
    void sendServiceThreadFunc();
    void loginServiceThreadFunc(int socketClientFd, sockaddr_in clientAddr);

private:
   std::vector<ClientInfo> m_clientsList;
   int m_listenSocket; 
   std::mutex m_clientsMutex;
   std::mutex m_messageMutex;
   std::unique_ptr<std::thread> m_ptrSenderThread;
   std::vector <std::pair<int,std::string>> messageQueue; 
};

#endif //CHAT_SERVER_H