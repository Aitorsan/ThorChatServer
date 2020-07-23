#include "JsonFormatter.h"
#include "base64.h"
#include "nlohmann/json.hpp"
#include "ClientInfo.h"
#include <vector>
#include <sstream>
#include <iomanip>

// for convenience
using json = nlohmann::json;
/*  Json format

      If its a message
      ------------------
       {
           "sender":"sender name"
           "type":"message "
           "data:":"anything"
       }

      If its an image
      ----------------
         {
           "sender":"sender name"
           "type":"image"
           "filename":"image file name and extension"
           "data:":"image data"
       }
  */
 
std::string JsonFormatter::formatData(const std::string& data, const std::string& sender, 
                                      MsgType type, const std::string& imageName)
{
    std::string jsonString{};
    if (type == MsgType::IMAGE)
        jsonString = createJsonImagePacket(imageName,sender,data);
    else if(type == MsgType::MESSAGE)
        jsonString = createJsonMsgPacket(sender,data);
    else if( type == MsgType::LOGIN)
        jsonString = createLogInPacket(sender,data);
    
    return jsonString;
}

 std::string JsonFormatter::createLogInPacket(const std::string& sender, const std::string& data)
 {
    json j;

    j["sender"] = sender;
    j["type"] = "login";
    j["data"]  = data;
    auto loginTime = std::time(nullptr);
    std::stringstream buff;
    buff << std::put_time(std::gmtime(&loginTime), "%d/%m/%y %T%z");
    j["time"] = buff.str();

    return j.dump();  

 }


 std::string JsonFormatter::createJsonImagePacket(const std::string& fileName,const std::string& sender, const std::string& data)
 {
    auto loginTime = std::time(nullptr);
    std::stringstream buff;
    buff << std::put_time(std::gmtime(&loginTime), "%d/%m/%y %T%z");
    std::vector<std::pair<std::string,std::string>>jsonData = {
    { "\"sender\"", std::string("\"")+sender+std::string("\",\n") },
    { "\"type\"", "\"message\",\n" },
     { "\"time\"", std::string("\"")+ buff.str() +std::string("\",\n") },
    { "\"filename\"", std::string("\"")+ fileName +std::string("\",\n") },
    { "\"data\"", std::string("\"")+ base64(data) +std::string("\"")},
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
std::string JsonFormatter::createJsonMsgPacket( const std::string& sender, const std::string& message)
{

    auto loginTime = std::time(nullptr);
    std::stringstream buff;
    buff << std::put_time(std::gmtime(&loginTime), "%d/%m/%y %T%z");

   std::vector<std::pair<std::string,std::string>>jsonData = {
    { "\"sender\"", std::string("\"")+sender+std::string("\",\n") },
    { "\"type\"", "\"message\",\n" },
    { "\"time\"", std::string("\"")+ buff.str() +std::string("\",\n") },
    { "\"data\"", std::string("\"")+ base64(message) +std::string("\"")},
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


void JsonFormatter::parseLoginInfo( ClientInfo& clientInfo,const std::string& jsonString)
{
    // parse explicitly
    auto j = json::parse(jsonString);
    
    auto passIt = j.find("password");
    auto usernameIt = j.find("sender");
    
    if (passIt != j.end() && usernameIt != j.end()) 
    {
        clientInfo.password = passIt.value();
        clientInfo.name = usernameIt.value();
    }

}