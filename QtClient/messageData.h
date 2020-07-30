/*  30-06-2020

    this struct contains all the data parsed from the json files. In this way we 
    avoid having thousands of parameters. The message struct contains all the fields
    of the json messages but only the ones that are releveant for the type of message
    are filled.

// json format for the messages
{	
   "type": "message,image,user-joined,user-left,login,signup",
   "sender": "sender name",
   "id":"id hash"
   "time": "time stamp when this message was sent",
   "image":"base64 embeded image used for profile"
   "image-extension": ".jpg , .png"
   "text": "the message"
}

*/
#ifndef MESSAGE_DATA_H
#define MESSAGE_DATA_H

#include <QString>


enum MsgType
{
   IMAGE,
   MESSAGE,
   LOGIN,
   SIGN_UP,
   USER_JOINED,
   USER_LEFT,
};


struct MessageData
{  
   QString m_type{};
   QString m_sender{};
   QString m_id{};
   QString m_time{};
   QString m_base64Image{"none"};
   QString m_imageExtension{"none"};
   QString m_text{};
};
#endif //MESSAGE_DATA_H