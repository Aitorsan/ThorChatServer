#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include "chatclient.h"
#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class ChatWindow;}
QT_END_NAMESPACE

class QStandardItemModel;
class CustomTextEdit;
class ChatWindow: public QMainWindow
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatWindow)
public:
	explicit ChatWindow(QStandardItemModel& model,QWidget* parent = nullptr );
    ~ChatWindow();
    void loadHtmlConversation(const QString& html);

signals:
	void userVerified();

public slots:
	void sendMessage(); 
	void msgReceived(const QString& sender,const QString &msg,const QString& time,const QString& image);
    void connectToServer(const QString&, const QString&);
	void sendLogInData();
private:
	QString buildHtmlRecievedMsgTags(const QString &username, const QString &message,const QString& time, const QString &userImage);
	QString buildLocalHtmlTags(const QString& userName,const QString& message, const QString& time,const QString& userImage);

	Ui::ChatWindow* ui;
	CustomTextEdit* m_customTextEdit;
	QStandardItemModel& m_chatModel;
	ChatClient* m_chatClient;
	QString m_userName;
  	QString m_password;
	QVector<QString> m_htmlDocList;
	QString m_stylesDir;

};
#endif
		
