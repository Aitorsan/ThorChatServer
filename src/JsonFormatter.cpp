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

    
  */

std::string JsonFormatter::formatData(const std::string &data, const std::string &sender,
                                      MsgType type, const std::string &imageName)
{
    std::string jsonString{};
    if (type == MsgType::IMAGE)
        jsonString = createJsonImagePacket(imageName, sender, data);
    else if (type == MsgType::LOGIN_RESPONSE)
        jsonString = createLogInPacket(sender, data);
    else if (type == MsgType::USER_JOINED)
        jsonString = createNewUserJoinedPacket(sender, data);
     else if (type == MsgType::USER_LEFT)
        jsonString = createUserLeftPacket(sender, data);

    return jsonString;
}

std::string JsonFormatter::createNewUserJoinedPacket(const std::string &sender, const std::string &data)
{
    auto loginTime = std::time(nullptr);
    std::stringstream buff;
    buff << std::put_time(std::gmtime(&loginTime), "%d/%m/%y T%");
   
    json j;
    j["sender"] = sender;
    j["id"] = "none";//TODO: for now
    j["type"] = "user-joined";
    j["text"] = data;
    j["image"] = "none";
    j["image-extension"] = "none";
    j["time"] = buff.str();
  
  return j.dump();
}

std::string JsonFormatter::createLogInPacket(const std::string &sender, const std::string &data)
{
    auto loginTime = std::time(nullptr);
    std::stringstream buff;
    buff << std::put_time(std::gmtime(&loginTime), "%d/%m/%y"); 
    
    json j;
    j["sender"] = sender;
    j["id"] = "none";//TODO: temp fix
    j["type"] = "login";
    j["text"] = data;
    j["time"] = buff.str();
    j["image"] = "none";
    j["image-extension"] = "none";

    return j.dump();
}

std::string JsonFormatter::createJsonImagePacket(const std::string &fileName, const std::string &sender, const std::string &data)
{
    auto loginTime = std::time(nullptr);
    std::stringstream buff;
    buff << std::put_time(std::gmtime(&loginTime), "%d/%m/%y T%");

    json j;
    j["sender"] = sender;
    j["type"] = "message";
    j["filename"] = fileName;
    j["text"] =  base64(data);
    j["image"] = "none";
    j["image-extension"] = "none";
    j["time"] = buff.str();

    return j.dump();
}



void JsonFormatter::parseLoginInfo(ClientInfo &clientInfo, const std::string &jsonString)
{
    // parse explicitly
    auto j = json::parse(jsonString);

    auto passIt = j.find("text");
    auto usernameIt = j.find("sender");

    if (passIt != j.end() && usernameIt != j.end())
    {
        clientInfo.password = passIt.value();
        clientInfo.name = usernameIt.value();
    }
}


std::string JsonFormatter::createUserLeftPacket(const std::string& sender,const std::string& data)
{
     auto loginTime = std::time(nullptr);
    std::stringstream buff;
    buff << std::put_time(std::gmtime(&loginTime), "%d/%m/%y T%");

    json j;
    j["sender"] = sender;
    j["id"] = "none"; //TODO: when database works
    j["type"] = "user-left";
    j["text"] =  data;
    j["image"] = "none";
    j["image-extension"] = "none";
    j["time"] = buff.str();

    return j.dump();
}
