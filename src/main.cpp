#include "net_func_wrappers.h"
#include "ClientInfo.h"
#include "json11.hpp"
#include "base64.h"
#include "chatServerEngine.h"
#include <time.h>
#include <vector>
#include <thread>
#include <algorithm>
#include <functional>
#include <mutex>
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
   
   json11::Json my_json = json11::Json::object {
    { "type", "image" },
    {"filename",fileName},
    { "extension", imageTypeExtension },
    { "data", base64(data) }
};
   std::string json_str = my_json.dump();
   
   return json_str;
  
}

int main()
{

   ChatServerEngine server;

   server.initializeServer(SERVER_PORT);


   std::string image = ReadImage();
   std::string jsonPacket = createImagePacket("somefile","jpg",image);
   //SaveFile(jsonPacket);
    std::cout << jsonPacket << std::endl;
 
   server.run();

return 0;
}
