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

ChatClient::ChatClient(QObject *parent)
    : QObject(parent), m_clientSocket(new QTcpSocket(this)), m_loggedIn(false)
{
    // Forward the connected and disconnected signals
    connect(m_clientSocket, &QTcpSocket::connected, this, &ChatClient::connected);
    connect(m_clientSocket, &QTcpSocket::connected, this, &ChatClient::sendLogInData);
    connect(m_clientSocket, &QTcpSocket::disconnected, this, &ChatClient::disconnected);
    
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

    //m_clientSocket->waitForConnected(2000);
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

QString ChatClient::formatMessage(const QString &username, const QString &time, const QString &data)
{
    QJsonObject jsonMessage;
    jsonMessage.insert("type", QJsonValue::fromVariant("message"));
    jsonMessage.insert("sender", QJsonValue::fromVariant(username));
    jsonMessage.insert("data", QJsonValue::fromVariant(data));
    jsonMessage.insert("time", QJsonValue::fromVariant(time));

    QJsonDocument doc(jsonMessage);
    qDebug() << doc.toJson();

    return doc.toJson();
}

QString ChatClient::formatLogginMessage(const QString &username, const QString &password, const QString &time)
{
    QJsonObject logginObject;
    logginObject.insert("type", QJsonValue::fromVariant("login"));
    logginObject.insert("sender", QJsonValue::fromVariant(username));
    logginObject.insert("password", QJsonValue::fromVariant(password));
    logginObject.insert("time", QJsonValue::fromVariant(time));

    QJsonDocument doc(logginObject);
    qDebug() << doc.toJson();

    return doc.toJson();
}
void ChatClient::ParseData(const QJsonObject &docObj)
{
    /*  Json format

      If its a message
      ------------------
       {
           "sender":"sender name"
           "time":"time"
           "type":"message "
           "data:":"anything"
       }

      If its an image
      ----------------
        {
           "sender":"sender name"
           "type":"image"
           "time": "time"
           "filename":"image file name and extension"
           "data:":"image data"
        }
  */

    // actions depend on the type of message
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    const QJsonValue sentTimeVal = docObj.value(QLatin1String("time"));
    const QJsonValue senderVal = docObj.value(QLatin1String("sender"));

    if (typeVal.isNull() || !typeVal.isString())
    {
        qDebug() << "ERROR DATA: type field";
        return; // a message with no type was received so we just ignore it
    }

    if (senderVal.isNull() || !senderVal.isString())
    {
        qDebug() << "ERROR DATA: sender field";
        return; // a message with no type was received so we just ignore it
    }

    if (sentTimeVal.isNull() || !sentTimeVal.isString())
    {
        qDebug() << "ERROR DATA: time field error";
        return;
    }

    QString sentTime = sentTimeVal.toString();
    QString sender = senderVal.toString();

    if (typeVal.toString().compare(QLatin1String("image"), Qt::CaseInsensitive) == 0)
    {
        qDebug() << "Is image";
        const QJsonValue filename = docObj.value(QLatin1String("filename"));
        const QJsonValue imageVal = docObj.value(QLatin1String("data"));

        if (imageVal.isNull() || !imageVal.isString())
        {
            qDebug() << "ERROR DATA: image data";
            return;
        }
        if (filename.isNull() || !filename.isString())
        {
            qDebug() << "ERROR DATA: filname field";
            return;
        }

        QByteArray text = QByteArray::fromBase64(imageVal.toVariant().toByteArray());

        std::string imageData = std::move(text.toStdString());
        std::string fileName = std::move(filename.toString().toStdString());
        saveImage(fileName, imageData);
    }
    else if (typeVal.toString().compare(QLatin1String("message"), Qt::CaseInsensitive) == 0)
    { //It's a login message
        qDebug() << "Is message";
        const QJsonValue dataVal = docObj.value(QLatin1String("data"));
        if (dataVal.isNull() || !dataVal.isString())
        {
            qDebug() << "ERROR DATA";
            return;
        }

        const QJsonValue imageVal = docObj.value(QLatin1String("image"));
        if (imageVal.isNull() || !dataVal.isString())
        {
            //TODO:for now nothing
        }

        emit messageReceived(sender, dataVal.toString(), sentTime, "none");
    }
    else if (typeVal.toString().compare(QLatin1String("login"), Qt::CaseInsensitive) == 0)
    { //It's a login message
        qDebug() << "Is login";
        const QJsonValue dataVal = docObj.value(QLatin1String("data"));
        if (dataVal.isNull() || !dataVal.isString())
        {
            qDebug() << "ERROR DATA";
            return;
        }
        verifyLogIn(sender, dataVal.toString());
    }
    else if (typeVal.toString().compare(QLatin1String("user-joined"), Qt::CaseInsensitive) == 0)
    {   
        //new user joined the chat  message
        qDebug() << "new user joined";
        
        const QJsonValue dataVal = docObj.value(QLatin1String("data"));

        if (dataVal.isNull() || !dataVal.isString())
        {
            qDebug() << "ERROR DATA";
            return;
        }
        emit userJoined(sender);
    }
}

/* slot called by TCP socket when there is data to read*/
void ChatClient::onReadyRead()
{
    // prepare a container to hold the UTF-8 encoded we receive from the socket
    QByteArray textData{};

    qDebug() << "\n--------\nonReadyRead\n---------";

    // we start a transaction so we can revert to the previous state in case we try to read more data than is available on the socket
    m_clientSocket->startTransaction();
    textData = m_clientSocket->readAll();
    qDebug() << "size: " << textData.size();
    m_clientSocket->commitTransaction();
    m_clientSocket->rollbackTransaction();
    /// we successfully read some data
    // we now need to make sure it's in fact a valid JSON
    QJsonParseError parseError;
    //find and remove any extra characters at the end
    auto index = textData.indexOf('}');
    textData.remove(index + 1, textData.size());

    QDebug deb = qDebug();
    for (auto all : textData)
        deb << all;
    qDebug() << "";

    // we try to create a json document with the data we received
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(textData, &parseError);
    if (parseError.error == QJsonParseError::NoError)
    {
        qDebug() << "\n----\nValid json";
        // if the data was indeed valid JSON
        if (jsonDoc.isObject())          // and is a JSON object
            ParseData(jsonDoc.object()); // parse the JSON
    }
    else
    {
        qDebug() << parseError.errorString();
    }
    m_clientSocket->flush();
}

void ChatClient::verifyLogIn(const QString &sender, const QString &data)
{
    if (sender == "server")
    {
        if (data == "succeed")
            emit loggedIn();
        else
        {
        }
    }
}

void ChatClient::sendMessage(const QString &text)
{
    m_clientSocket->startTransaction();
    m_clientSocket->write(text.toStdString().c_str());
    m_clientSocket->rollbackTransaction();
}
