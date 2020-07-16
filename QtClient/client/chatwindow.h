#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include "chatclient.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class ChatWindow;}
QT_END_NAMESPACE

class QStandardItemModel;
class QString;

class ChatWindow: public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatWindow)
public:
	explicit ChatWindow(QStandardItemModel& model,QWidget* parent = nullptr );
    ~ChatWindow();
    void loadHtmlConversation();

signals:
	void userVerified();

public slots:
	void sendMessage();
   void msgReceived( const QString& msg);
   void connectToServer();
private:
    void buildHtmlSendTags(const QString& username, const QString& message, const QString& userImage);
	void buildLocalHtmlTags(const QString& userName,const QString& message, const QString& userImage);

	Ui::ChatWindow* ui;
	QStandardItemModel& m_chatModel;
	ChatClient* m_chatClient;
	QVector<QString> m_htmlDoc;
};
#endif
		
