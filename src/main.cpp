
#include "chatServerEngine.h"
#include "net_func_wrappers.h"
#include <time.h>
#include <sstream>
#include <fstream>


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


int main()
{

   ChatServerEngine server;

   server.initializeServer(SERVER_PORT);


   std::string image = ReadImage();

   server.run();

return 0;
}
