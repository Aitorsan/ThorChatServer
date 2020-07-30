#ifndef JSON_FORMATTER_H
#define JSON_FORMATTER_H

#include "IFormatter.h"

class JsonFormatter: public IFormatter 
{

public:
    JsonFormatter() = default;
    virtual ~JsonFormatter(){};
    virtual std::string formatData(const std::string& data, 
                                   const std::string& sender, 
                                   MsgType type, 
                                   const std::string& imageName = std::string("none")) override ;
    virtual void parseLoginInfo( ClientInfo& clientInfo,const std::string& data)override;
private:
    std::string createJsonImagePacket(const std::string& fileName,const std::string& sender, const std::string& data);
    std::string createLogInPacket(const std::string& sender, const std::string& data);
    std::string createNewUserJoinedPacket(const std::string& sender, const std::string& data);
    std::string createUserLeftPacket(const std::string& sender,const std::string& data);
};
#endif