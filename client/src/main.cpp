#include <iostream>
#include "net_func_wrappers.h"
#include <string>
#include <regex>
std::string getUserName()
{
    std::string userName{};
    std::regex strExpr ("[a-z]+");
	while(true)
	{
       std::cout << "Please introduce your name:\n>";
	   std::getline(std::cin,userName);
       std::smatch m;
	   if( std::regex_match (userName,m,strExpr))
			break;
	}
	return userName;
}

int main(int argc , char **argv)
{
	
	if ( argc != 2)
		err_quit("Introduce ip address!\n");

    std::cout << "server IP:" << argv[1] << std::endl;

    std::string userName = getUserName();

	//1. create a socket file descriptor
	int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	 // ctr+c signal
   setSignalHandler(SIGINT, [](int signo)
   {
	   exit(0);
   });
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

    // set non blocking socket
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	send(sockfd,userName.c_str(),userName.size(),0);
	sleep(1);
    //loop and read the server messages
	while (!closeConnection)
	{
       //In non-blocking mode recv will return immediately if there is zero bytes 
       //of data to be read and will return -1, setting errno to EAGAIN or EWOULDBLOCK.
	   int bytesReceived = recv(sockfd,(void*)&buffer,BUFFSIZE, 0);
	   
	   if ( bytesReceived < 0 )
       {
           switch ( errno )
		   {
             case EINTR :
			 case EAGAIN :
			  // chek if the read system call was interrupted
             break;
			 default:
                 //we have an error
				std::cout << "errno:" << errno << std::endl;
				err_sys("fatal error reciving data");
				break;
		   };
        }
		//std::cout <<"bytes Received:"<< bytesReceived <<std::endl;
	   std::cout << buffer << "["<< userName<<"]:>"<< std::flush;
	   memset(buffer,0,sizeof(buffer));
       std::string message{};
       std::getline(std::cin,message);
	   send(sockfd,message.c_str(),message.size(),0);
	   
	   if( message == "quit")
	        closeConnection = true;
	   
	}

    // make sure nothing is left in the buffer to read
	if( recv(sockfd,(void*)&buffer,BUFFSIZE, 0) > 0)
	{
		std::cout << buffer << std::endl;
	}
	// close the connection
    Close(sockfd);
	return 0;
}
