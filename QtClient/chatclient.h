#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include "messageData.h"
#include <QObject>
#include <QAbstractSocket>
class QHostAddress;
class QTcpSocket;
class QJsonObject;
class QVariant;

class ChatClient : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatClient)
public:
    explicit ChatClient(QObject *parent = nullptr);
    void saveImage(std::string& name, const std::string& imageData );
    void ParseData(const QJsonObject& docObj);
    QString formatMessage(MessageData& msg, const MsgType &type);
    QString decodeBase64Data(const QVariant& base64data);

public slots:
    void connectToServer(const QHostAddress &address, quint16 port);
    void sendMessage(const QString &text);
    void disconnectFromHost();
    void onReadyRead();
  
signals:
    void connected();
    void sendLogInData();
    void loggedIn();
    void loginError(const QString &reason);
    void disconnected();
    void messageReceived(const MessageData &);
    void error(QAbstractSocket::SocketError socketError);
    void userJoined(const MessageData &username);
    void userLeft(const MessageData &username);

private:
    void verifyLogInStatus(const MessageData& msg);

    QTcpSocket* m_clientSocket;
    bool m_loggedIn;
};

#endif