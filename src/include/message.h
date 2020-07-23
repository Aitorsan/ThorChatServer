#ifndef MESSAGE_H
#define MESSAGE_H

#include "IFormatter.h"
#include <memory>


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
class Message
{
public:

    Message(MsgType type, const std::string& sender,
                           const std::string& data, 
                           const std::string& imageName = std::string("none"));
    std::string getFormattedMessage();

private:
    std::string m_senderName;
    std::string m_message;
    std::string m_imageFilename;
    MsgType m_type;
    std::unique_ptr<IFormatter> m_formatter;
};

#endif