#include "net_func_wrappers.h"
#include <iostream>
#include <time.h>
bool closeSocket{false};
int listen_fd;
void close_socket(int sock)
{
     close(listen_fd);
}
int main()
{
    //create listening socket file descriptor
    listen_fd = Socket(AF_INET, SOCK_STREAM, 0);
    //client address
    sockaddr_in client_address;
    //server address
    sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(1333);/*daytime server*/
   
   // bind server socket
   Bind(listen_fd, ( sockaddr*)&server_address, sizeof(server_address));

   std::cout << "listening on:"<< ntohl(server_address.sin_addr.s_addr) << ": "<< 1333 << std::endl;

   //Listen
   Listen(listen_fd,5);

   // client file descriptor
   int client_fd;

   //buffer for the message
   char buffer[MAXLINE];

   // set ctr+c signal to close the socket
    setup_ctrl_c_handler(close_socket);

   while(true)
   {
       socklen_t len = sizeof(client_address);
       //Accept client
       client_fd = Accept(listen_fd,(sockaddr*)&client_address,&len);
       std::cout << "conecction from: "<< inet_ntop(AF_INET, &client_address.sin_addr,buffer, sizeof(buffer))<< ", port: "<< ntohs(client_address.sin_port)<<std::endl;

       snprintf(buffer, sizeof(buffer), "Rahul You are connected to the chat server\r\n");
       
       //Send message back to the client 
       write(client_fd,buffer, strlen(buffer));

       //Close the connection
       close(client_fd);
   }

   
	
return 0;
}
