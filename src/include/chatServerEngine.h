#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include "ClientInfo.h"
#include <vector>
#include <mutex>

struct sockaddr_in;

class ChatServerEngine
{
 public:
    ChatServerEngine() = default;
    ~ChatServerEngine();

    void initializeServer( int portNumber,const char* ipAddress = nullptr);
    bool run();

private:
    void installSignalHandlers();
    int createServerSocket( int portNumber,const char* serverAddress);
    void addNewClient( const sockaddr_in& client_address, int clientSocketFileDescriptor);
    void startNewServiceThread();
    void findAndRemoveClient(const ClientInfo& clientToClose);
    void sendMsgToClients(int senderFd, const std::string& message);
    void sendMsgTo(int clientSocketFd,const std::string& message);

    //threaded service
    void serviceClient();

private:
   sockaddr_in* m_clientAddress;
   std::vector<ClientInfo> m_clientsList;
   int m_listenSocket; 
   std::mutex m_mutex;
};

#endif //CHAT_SERVER_H