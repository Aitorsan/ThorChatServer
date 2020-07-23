#ifndef IFORMATTER_H
#define IFORMATTER_H
#include <string>
enum MsgType
{
   IMAGE,
   MESSAGE,
   LOGIN
   
};

class ClientInfo;

class IFormatter
{
public:
    virtual ~IFormatter(){};
    virtual std::string formatData(const std::string& data, 
                                   const std::string& sender, 
                                   MsgType type, 
                                   const std::string& imageName = std::string("none")) = 0;

   virtual void parseLoginInfo( ClientInfo& clientInfo,const std::string& data) = 0;

};

#endif //IFORMATTER_H