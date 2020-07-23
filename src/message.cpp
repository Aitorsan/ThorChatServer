#include "message.h"
#include "JsonFormatter.h"

Message::Message(MsgType type, const std::string& sender, const std::string& data, const std::string& imageName )
        : m_senderName{sender}
        , m_message{data}
        , m_imageFilename{imageName}
        , m_type{type}
        , m_formatter{new JsonFormatter}
{
}

std::string Message::getFormattedMessage()
{
    return m_formatter->formatData(m_message,m_senderName,m_type,m_imageFilename);
}