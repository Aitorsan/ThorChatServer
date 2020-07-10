#include <iostream>
#include "net_func_wrappers.h"
#include <string>
#include <regex>
#include <atomic>
#include <thread>
#include <mutex>
#include <functional>

enum TextColor
{
	Red = 31,
	Green = 32,
	Yellow = 33,
	Blue = 34,
	Magenta = 35,
	Cyan = 36,
	White = 37,
	BBlack = 90,  
	BRed = 91, 
	BGreen = 92,
	BYellow = 93,
	BBlue = 94 ,
	BMagenta = 95,
	BCyan = 96  
};

const std::map<std::string,TextColor> colorMap{

	{"red", TextColor::Red},
	{"green", TextColor::Green},
	{"yellow",TextColor::Yellow},
	{"blue" ,TextColor::Blue},
	{"magenta" , TextColor::Magenta},
	{"cyan",TextColor::Cyan},
	{"white" ,TextColor::White},
	{"bright_black",TextColor::BBlack},  
	{"bright_red" ,TextColor::BRed}, 
	{"bright_green",TextColor::BGreen},
	{"bright_yellow", TextColor::BYellow},
	{"bright_blue",TextColor::BBlue },
	{"bright_magenta", TextColor::BMagenta},
	{"bright_cyan" ,TextColor::BCyan} 
};

std::atomic<bool> closeConnection{false};//exit flag
std::mutex m;

TextColor getPickedTextColor()
{
	std::string textColor{};
    std::regex strExpr ("[a-z]+8");
    TextColor color;
	auto colorMapBeg = colorMap.cbegin();
	auto colorMapEnd = colorMap.cend();

    std::string colorList{};

	for (; colorMapBeg != colorMapEnd; ++colorMapBeg)
	{
		colorList+= colorMapBeg->first + '\n';
	}

	while(true)
	{
       std::cout << "Please choose a color for the text:\n\x1B[32m"<< colorList << "\033[0m\n>";
	   std::getline(std::cin,textColor);

	   auto found = colorMap.find(textColor);
	   
	   if( found != colorMapEnd)
	   {
		   color = found->second;
			break;
	   }
	   
	}
	return color;
}

void printInputRightArrow(TextColor color,const char* userName)
{
	std::lock_guard<std::mutex> lockguard(m);
	std::cout << "\x1B["<<std::to_string(color)<<"m"<<"["<< userName<<"]:>"<<"\033[0m"<< std::flush;
}

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

void getUserInput(int sockfd, std::string userName, TextColor choosedColor)
{
	auto printInRightArrow = std::bind(&printInputRightArrow, choosedColor,std::placeholders::_1);

	while(true)
	{
	   std::string message{};
       std::getline(std::cin,message);
	   send(sockfd,message.c_str(),message.size(),0);
	   
	   if( message == "quit")
	   {
	        closeConnection = true;
            break;
	   }
	   
	   printInRightArrow(userName.c_str());
	}

	close(sockfd);
}

int main(int argc , char **argv)
{
	
	if ( argc != 2)
		err_quit("Introduce ip address!\n");

    std::cout << "server IP:" << argv[1] << std::endl;

    std::string userName = getUserName();
	TextColor color = getPickedTextColor();

	auto printInRightArrow = std::bind(&printInputRightArrow, color,std::placeholders::_1);

	//1. create a socket file descriptor
	int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	// ctr+c signal
    setSignalHandler(SIGINT, [](int signo)
    {
	   exit(0);
    });
	// ctr+c signal
    setSignalHandler(SIGTERM, [](int signo)
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

	send(sockfd,userName.c_str(),userName.size(),0);
	sleep(1);
	
	//input thread
	std::thread inputThread(getUserInput,sockfd, userName,color);
	inputThread.detach();
    //loop and read the server messages
	while (!closeConnection)
	{
       //In non-blocking mode recv will return immediately if there is zero bytes 
       //of data to be read and will return -1, setting errno to EAGAIN or EWOULDBLOCK.
	   int bytesReceived = recv(sockfd,(void*)&buffer,BUFFSIZE, 0);
	   
	   if ( bytesReceived < 0 )
       {
           if ( errno != EINTR )
           {
			   //we have an error
			   std::cerr << "errno:" << errno << std::endl;
			   err_sys("fatal error reciving data");
		   }         
        }
		
	   m.lock();
	   std::cout << buffer <<std::flush; 
	   m.unlock();
	   printInRightArrow(userName.c_str());
	   memset(buffer,0,sizeof(buffer));
	}
	
    // make sure nothing is left in the buffer to read
	if( recv(sockfd,(void*)&buffer,BUFFSIZE, 0) > 0)
	{
		m.lock();
		std::cout << buffer << std::endl;
		m.unlock();
	}

	// close the connection
    shutdown(sockfd,SHUT_RDWR);
	return 0;
}
