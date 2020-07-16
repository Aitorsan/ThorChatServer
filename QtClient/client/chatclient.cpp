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
    : QObject(parent)
    , m_clientSocket(new QTcpSocket(this))
    , m_loggedIn(false)
{
    // Forward the connected and disconnected signals
    connect(m_clientSocket, &QTcpSocket::connected, this, &ChatClient::connected);
    connect(m_clientSocket, &QTcpSocket::disconnected, this, &ChatClient::disconnected);
    // connect readyRead() to the slot that will take care of reading the data in
    connect(m_clientSocket, &QTcpSocket::readyRead, this, &ChatClient::onReadyRead);
    // Forward the error signal, QOverload is necessary as error() is overloaded, see the Qt docs
    connect(m_clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ChatClient::error);
    // Reset the m_loggedIn variable when we disconnect. Since the operation is trivial we use a lambda instead of creating another slot
    connect(m_clientSocket, &QTcpSocket::disconnected, this, [this]()->void{m_loggedIn = false;});

}


/*slot called by the GUI*/
void ChatClient::connectToServer( const QHostAddress& address, quint16 port)
{
    m_clientSocket->connectToHost(address,port);
    
    //m_clientSocket->waitForConnected(2000);
}

/*slot called by the GUI*/
void ChatClient::disconnectFromHost()
{
    m_clientSocket->disconnectFromHost();
}

void ChatClient::saveImage(std::string& name, const std::string& type, const std::string& imageData )
{
    std::string resPath {"res/styles/"};
    std::istringstream buffer(imageData);
    name+=".";
    name+=type;
    resPath+= name;
    std::ofstream(resPath, std::ios::binary) << buffer.rdbuf();
        
}
void ChatClient::ParseData(const QJsonObject& docObj)
{
    /*  Jason format

      If its a message
      ------------------
       {
           "type":"message "
           "data:":"anything"
       }

      If its an image
      ----------------
         {
           "type":"image "
           "filename":"<file name>"
           "extension":".jpg, .png"
           "data:":"image data"
       }
  */

    // actions depend on the type of message
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    if (typeVal.isNull() || !typeVal.isString())
    {
        qDebug() << "ERROR DATA";
        return; // a message with no type was received so we just ignore it

    }
    if (typeVal.toString().compare(QLatin1String("image"), Qt::CaseInsensitive) == 0)
    {
        qDebug() << "Is image";
        const QJsonValue filenameVal = docObj.value(QLatin1String("filename"));
        const QJsonValue extensionVal = docObj.value(QLatin1String("extension"));
        const QJsonValue imageVal = docObj.value(QLatin1String("data"));

        if (imageVal.isNull() || !imageVal.isString())
        {
            qDebug() << "ERROR DATA";
            return;

        }
        if (filenameVal.isNull() || !filenameVal.isString())
        {
            qDebug() << "ERROR DATA";
            return;
        }
            
        if (extensionVal.isNull() || !extensionVal.isString())
        {
            qDebug() << "ERROR DATA";
            return;
        }
        
        QByteArray text =  QByteArray::fromBase64(imageVal.toVariant().toByteArray());      
               
        std::string imageData = std::move(text.toStdString());
        std::string fileName = std::move(filenameVal.toString().toStdString());
        std::string imageType = std::move(extensionVal.toString().toStdString());

        saveImage(fileName, imageType, imageData);
    }
    else if (typeVal.toString().compare(QLatin1String("data"), Qt::CaseInsensitive) == 0)
    { //It's a login message
        qDebug() << "Is message";
        const QJsonValue dataVal = docObj.value(QLatin1String("data"));
        if (dataVal.isNull() || !dataVal.isString())
        {
            qDebug() << "ERROR DATA";
            return;

        }

        emit messageReceived(dataVal.toString());
    }
}

/* slot called by TCP socket when there is data to read*/
void ChatClient::onReadyRead()
{
    // prepare a container to hold the UTF-8 encoded we receive from the socket
    QByteArray textData;

    qDebug() << "onReadyRead";
     QDataStream socketStream(m_clientSocket);
    // set the version so that programs compiled with different versions of Qt can agree on how to serialise
    
    // start an infinite loop

        // we start a transaction so we can revert to the previous state in case we try to read more data than is available on the socket
        m_clientSocket->startTransaction();
        textData = m_clientSocket->readAll() ;
        qDebug() << "size: "<< textData.size();
        m_clientSocket->rollbackTransaction();
            /// we successfully read some data
            // we now need to make sure it's in fact a valid JSON
            QJsonParseError parseError;
            // we try to create a json document with the data we received
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(textData, &parseError);
           if (parseError.error == QJsonParseError::NoError)
            {
                qDebug() << "Valid json";
                // if the data was indeed valid JSON
                if (jsonDoc.isObject())          // and is a JSON object
                    ParseData(jsonDoc.object()); // parse the JSON
            }
            
}


void ChatClient::login(const QString &userName)
{
}

void ChatClient::sendMessage(const QString &text)
{
    m_clientSocket->startTransaction();
    m_clientSocket->write(text.toStdString().c_str());
    m_clientSocket->rollbackTransaction();
}
