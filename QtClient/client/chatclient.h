#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QAbstractSocket>

class QHostAddress;
class QTcpSocket;
class QJsonObject;

class ChatClient : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatClient)
public:
    explicit ChatClient(QObject *parent = nullptr);
    void saveImage(std::string& name, const std::string& type, const std::string& imageData );
    void ParseData(const QJsonObject& docObj);

public slots:
    void connectToServer(const QHostAddress &address, quint16 port);
    void login(const QString &userName);
    void sendMessage(const QString &text);
    void disconnectFromHost();

private slots:
    void onReadyRead();
  
signals:
    void connected();
    void loggedIn();
    void loginError(const QString &reason);
    void disconnected();
    void messageReceived(const QString &text);
    void error(QAbstractSocket::SocketError socketError);
    void userJoined(const QString &username);
    void userLeft(const QString &username);
private:
    QTcpSocket* m_clientSocket;
    bool m_loggedIn;
};

#endif