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
    void saveImage(std::string& name, const std::string& imageData );
    void ParseData(const QJsonObject& docObj);
    void verifyLogIn(const QString& sender,const QString& data);
    QString formatLogginMessage(const QString& username,const QString& password,const QString& time);
    QString formatMessage(const QString& username,const QString& time,const QString& html);


public slots:
    void connectToServer(const QHostAddress &address, quint16 port);
    void sendMessage(const QString &text);
    void disconnectFromHost();


private slots:
    void onReadyRead();
  
signals:
    void connected();
    void sendLogInData();
    void loggedIn();
    void loginError(const QString &reason);
    void disconnected();
    void messageReceived(const QString& sender,const QString &text,const QString& time, const QString& image);
    void error(QAbstractSocket::SocketError socketError);
    void userJoined(const QString &username);
    void userLeft(const QString &username);
private:
    QTcpSocket* m_clientSocket;
    bool m_loggedIn;
};

#endif