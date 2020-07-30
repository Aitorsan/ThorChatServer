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
class MessageData;
class ChatWindow: public QMainWindow
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatWindow)
public:
	explicit ChatWindow(QWidget* parent = nullptr );
    ~ChatWindow();
    void loadHtmlConversation(const QString& html);

signals:
	void userVerified();
	void userVerifyError(const QString &);
public slots:
	void sendMessage(); 
	void msgReceived(const MessageData& msg);
    void connectToServer(const QString&, const QString&);
	void sendLogInData();
	void addUserToListView(const MessageData&);
	void removeUserFromListView(const MessageData& userLeft);
	void loadProfileImage();

private:
	QString buildHtmlRecievedMsgTags(const MessageData &msg);
	QString buildLocalHtmlTags(const MessageData& msg);

	Ui::ChatWindow* ui;
	CustomTextEdit* m_customTextEdit;
    QStandardItemModel* m_chatModel;
	ChatClient* m_chatClient;
	QString m_userName;
  	QString m_password;
	QString m_image;
	QVector<QString> m_htmlDocList;
	QString m_stylesDir;
};
#endif
		
