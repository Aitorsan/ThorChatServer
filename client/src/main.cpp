#include <iostream>
#include "net_func_wrappers.h"

int main(int argc , char **argv)
{
	 // ctr+c signal
   setSignalHandler(SIGINT, [](int signo)
   {
      //just exit this will close all the sockets open
      exit(0);
   });
	if ( argc != 2)
		err_quit("Introduce ip address!\n");

    std::cout << "server IP:" << argv[1] << std::endl;
	//1. create a socket file descriptor
	int sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    //2. fill in the sockaddr structure 
	struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(SERVER_PORT);
    /*
	 This function converts the character string src into a network
     address structure in the af address family, then copies the network
     address structure to destinaton.
	*/
    inet_pton(AF_INET,argv[1],&server_address.sin_addr);
  
	//3. connect to the server
	Connect(sockfd,(struct sockaddr*)&server_address,sizeof(server_address));
	
	std::cout << "Connection stablished with server" << std::endl;
	char buffer[BUFFSIZE];

	bool closeConnection{false};
    //loop and read the server messages

	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	while(!closeConnection)
	{
      
 //In non-blocking mode recv will return immediately if there is zero bytes 
 //of data to be read and will return -1, setting errno to EAGAIN or EWOULDBLOCK.
	   int bytesReceived = recv(sockfd,(void*)&buffer,BUFFSIZE, 0);
	    if( bytesReceived < 0 )
         {
               if ( errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK )// chek if the read system call was interrupted
               {

			   }  
			   else
			   //we have an error
                  err_sys("fatal error reciving data");
         }
		//std::cout <<"bytes Received:"<< bytesReceived <<std::endl;
	   std::cout << buffer << std::endl << ":>"<< std::flush;
	   memset(buffer,0,sizeof(buffer));
       std::string message{};
       std::getline(std::cin,message);
	   if( message == "quit")
	        closeConnection = true;
	   
	   send(sockfd,message.c_str(),message.size(),0);
	}
	// close the connection
    Close(sockfd);
	return 0;
}
