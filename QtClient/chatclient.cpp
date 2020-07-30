#include "chatclient.h"
#include <QDataStream>
#include <QTcpSocket>
#include <QString>
#include <QHostAddress>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <sstream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>

ChatClient::ChatClient(QObject *parent)
    : QObject(parent), m_clientSocket(new QTcpSocket(this)), m_msgBuffer{}, m_loggedIn(false)
{
    // Forward the connected and disconnected signals
    connect(m_clientSocket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(m_clientSocket, SIGNAL(connected()), this, SIGNAL(sendLogInData()));
    connect(m_clientSocket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));

    // connect readyRead() to the slot that will take care of reading the data in
    connect(m_clientSocket, &QTcpSocket::readyRead, this, &ChatClient::onReadyRead);
    // Forward the error signal, QOverload is necessary as error() is overloaded, see the Qt docs
    connect(m_clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ChatClient::error);
    // Reset the m_loggedIn variable when we disconnect. Since the operation is trivial we use a lambda instead of creating another slot
    connect(m_clientSocket, &QTcpSocket::disconnected, this, [this]() -> void { m_loggedIn = false; });
}

/*slot called by the GUI*/
void ChatClient::connectToServer(const QHostAddress &address, quint16 port)
{
    // connect to the server
    m_clientSocket->connectToHost(address, port);
    m_clientSocket->waitForConnected(10000);
}

/*slot called by the GUI*/
void ChatClient::disconnectFromHost()
{
    m_clientSocket->disconnectFromHost();
}

void ChatClient::saveImage(std::string &name, const std::string &imageData)
{
    std::string resPath{"res/styles/"};
    std::istringstream buffer(imageData);

    resPath += name;
    std::ofstream(resPath, std::ios::binary) << buffer.rdbuf();
}

QString ChatClient::formatMessage(MessageData &msg, const MsgType &msgtype)
{
    // compute the time
    auto time = std::time(nullptr);
    std::stringstream buff;
    buff << std::put_time(std::gmtime(&time), "%d/%m/%y %T");

    // search message type
    QString type{"message"};

    switch (msgtype)
    {
    case IMAGE:
        type = "image";
        break;
    case MESSAGE:
        break;
    case LOGIN:
        type = "login";
        break;
    case SIGN_UP:
        type = "signup";
        break;
    case USER_JOINED:
        type = "user-joined";
        break;
    case USER_LEFT:
        type = "user-left";
        break;
    }

    msg.m_type = type; // this ensures that we did not type wrong the type
    msg.m_time = QString::fromStdString(buff.str());

    QJsonObject jsonMessage;
    jsonMessage.insert("type", QJsonValue::fromVariant(msg.m_type));
    jsonMessage.insert("id", QJsonValue::fromVariant(msg.m_id));
    jsonMessage.insert("sender", QJsonValue::fromVariant(msg.m_sender));
    jsonMessage.insert("text", QJsonValue::fromVariant(msg.m_text));
    jsonMessage.insert("time", QJsonValue::fromVariant(msg.m_time));
    jsonMessage.insert("image", QJsonValue::fromVariant(msg.m_base64Image));
    jsonMessage.insert("image-extension", QJsonValue::fromVariant(msg.m_imageExtension));

    QJsonDocument doc(jsonMessage);
    //qDebug() << doc.toJson();

    return doc.toJson();
}

void ChatClient::ParseData(const QJsonObject &docObj)
{
    /*  Json format
      ------------------
    {	
        "type": "message,image,user-joined,user-left,login,signup",
        "sender": "sender name",
        "time": "time stamp when this message was sent",
        "image":"base64 embeded image used for profile"
        "image-extension": ".jpg , .png"
        "text": "the message"
   }
  */

    /* this fields are compulsory if not the message will be ignored 
       if one of the fields are optional then they should contain 
       the word "none" to indicate that this value is blank
    */
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    const QJsonValue senderVal = docObj.value(QLatin1String("sender"));
    const QJsonValue idVal = docObj.value(QLatin1String("id"));
    const QJsonValue sentTimeVal = docObj.value(QLatin1String("time"));
    const QJsonValue imageVal = docObj.value(QLatin1String("image"));
    const QJsonValue imgExtensionVal = docObj.value(QLatin1String("image-extension"));
    const QJsonValue textDataVal = docObj.value(QLatin1String("text"));

    if (typeVal.isNull() || !typeVal.isString())
    { // a message with no type was received so we just ignore it
        qDebug() << "ERROR DATA: type field";
        return;
    }

    if (senderVal.isNull() || !senderVal.isString())
    { // a message with no sender was recevied
        qDebug() << "ERROR DATA: sender field";
        return;
    }

    if (idVal.isNull() || !idVal.isString())
    {
        qDebug() << "ERROR ID: Id field error";
        return;
    }

    if (sentTimeVal.isNull() || !sentTimeVal.isString())
    {
        qDebug() << "ERROR DATA: time field error";
        return;
    }

    if (textDataVal.isNull() || !textDataVal.isString())
    {
        qDebug() << "ERROR DATA: there is no text field";
        return;
    }

    if (imageVal.isNull() || !imageVal.isString())
    {
        qDebug() << "ERROR DATA: image data";
        return;
    }

    MessageData msg;
    msg.m_type = typeVal.toString();
    msg.m_sender = senderVal.toString();
    msg.m_id = idVal.toString();
    msg.m_time = sentTimeVal.toString();
    msg.m_base64Image = imageVal.toString();
    msg.m_imageExtension = imgExtensionVal.toString();
    msg.m_text = textDataVal.toString();

    const QString &msgType = msg.m_type;

    if (msgType.compare(QLatin1String("image"), Qt::CaseInsensitive) == 0)
    {
        qDebug() << "Is image";
        std::string fileName = "temp_name.jpg";
        saveImage(fileName, decodeBase64Data(imgExtensionVal.toVariant()).toStdString());
    }
    else if (msgType.compare(QLatin1String("message"), Qt::CaseInsensitive) == 0)
    {

        emit messageReceived(msg);
    }
    else if (msgType.compare(QLatin1String("login"), Qt::CaseInsensitive) == 0)
    {
        //It's a login message
        verifyLogInStatus(msg);
    }
    else if (msgType.compare(QLatin1String("user-joined"), Qt::CaseInsensitive) == 0)
    {
        //new user joined the chat  message
        qDebug() << "new user joined";

        emit userJoined(msg);
    }
    else if (msgType.compare(QLatin1String("user-left"), Qt::CaseInsensitive) == 0)
    {
        //new user joined the chat  message
        qDebug() << "user left joined";

        emit userLeft(msg);
    }
    m_msgBuffer.clear();
}

/* slot called by TCP socket when there is data to read*/
void ChatClient::onReadyRead()
{

    qDebug() << "\n--------\nonReadyRead\n---------";

    //if we find the curly brace it means that we have a complete message
    auto indexEnd = m_msgBuffer.indexOf('}');

    m_clientSocket->startTransaction();
    if (indexEnd == -1)
    {
        QByteArray temp = m_clientSocket->readAll();
        for( int i = 0; i < temp.size();++i)
        {
            m_msgBuffer.push_back(temp[i]);
        }
    }
    else
    {
        m_msgBuffer = m_clientSocket->readAll();
    }

    qDebug() << "size: " << m_msgBuffer.size();
    m_clientSocket->commitTransaction();
//if we find the curly brace it means that we have a complete message
    auto index = m_msgBuffer.indexOf('}');
    if (index != -1)
    {
        //remove newlines at the end or any weird character
        m_msgBuffer.remove(index + 1, m_msgBuffer.size());
        QDebug deb = qDebug();
        for (auto all : m_msgBuffer)
            deb << all;
        //make sure it's in fact a valid JSON
        QJsonParseError parseError;
        // we try to create a json document with the data we received
        const QJsonDocument jsonDoc = QJsonDocument::fromJson(m_msgBuffer, &parseError);
        if (parseError.error == QJsonParseError::NoError)
        {
            qDebug() << "\n----\nValid json\n----\n";
            // if the data was indeed valid JSON
            if (jsonDoc.isObject())          // and is a JSON object
                ParseData(jsonDoc.object()); // parse the JSON
        }
        else
        {
            qDebug() << parseError.errorString();
        }
    }
}

void ChatClient::verifyLogInStatus(const MessageData &msg)
{
    if (msg.m_sender == "server")
    {
        if (msg.m_text == "succeed")
            emit loggedIn();
        else
            emit loginError(msg.m_text);
    }
}

void ChatClient::sendMessage(const QString &text)
{
    m_clientSocket->startTransaction();
    m_clientSocket->write(text.toStdString().c_str());
    m_clientSocket->rollbackTransaction();
}

//Convinient helper to decode data into a QString
QString ChatClient::decodeBase64Data(const QVariant &data)
{
    QByteArray decodedDataArray = QByteArray::fromBase64(data.toByteArray());

    std::string decodedData = std::move(decodedDataArray.toStdString());

    return QString::fromStdString(decodedData);
}
