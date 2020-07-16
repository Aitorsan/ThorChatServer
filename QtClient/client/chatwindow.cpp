#include "ui_chatwindow.h"
#include "chatwindow.h"
#include <QString>
#include <QHostAddress>
#include <QStandardItemModel>
#include <QFile>
#include <QDebug>
#include <QUrl>
#include <QWebFrame>
#include <QIODevice>
#include <fstream>

ChatWindow::ChatWindow(QStandardItemModel &model, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatWindow)
    , m_chatModel(model)
    , m_chatClient(new ChatClient(this))
    , m_htmlDoc{}
{
  ui->setupUi(this);
  //QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
  //ui->webView->settings()->setObjectCacheCapacities(0, 0, 0);

  connect(m_chatClient, SIGNAL(messageReceived(const QString &)), this, SLOT(msgReceived(const QString &)));
  connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
  //connect(ui->lineEdit,SIGNAL(),this)
  /* connect(QWidget *centralwidget;
   connect(*actionSave_history;
    connect(QAction *actiondelete_history;
   connect( QGridLayout *gridLayout;
   connect( QVBoxLayout *verticalLayout;
    connect(QTextEdit *textEdit;
   connect( QLineEdit *lineEdit;
   connect( QListView *listView;
   connect( QMenuBar *menubar;
   connect( QMenu *menuFile;
   connect( QMenu *menuConnect;
   connect( QStatusBar *statusbar; */
}

ChatWindow::~ChatWindow()
{
  delete ui;
}



void ChatWindow::msgReceived(const QString &msg)
{
 

  qDebug() << "msgREceived";
 //Remove last 2 elements
    m_htmlDoc.removeLast();
    m_htmlDoc.removeLast();
    m_htmlDoc.push_back(msg);
    QString bodyEnd = "</body>";
    QString htmlEnd = "</html>";
    m_htmlDoc.push_back(bodyEnd);
    m_htmlDoc.push_back(htmlEnd);
  
 
  loadHtmlConversation();
}

void ChatWindow::loadHtmlConversation()
{
   QString text;
  
        for( auto all : m_htmlDoc)
        {
            text+= all;
            //file << all.toStdString()<<std::endl;
  
        }
   ui->webView->setHtml(text,QUrl("file:///home/aitor/Desktop/Projects/c++/ChatServer/QtClient/client/res/styles/index.html"));
  ui->webView->page()->mainFrame()->evaluateJavaScript("window.scrollTo(0, document.body.scrollHeight);");
}
void ChatWindow::connectToServer()
{
  m_chatClient->connectToServer(QHostAddress("0.0.0.0"), 55555);
  userVerified();
}

void ChatWindow::buildHtmlSendTags(const QString& username, const QString& message, const QString& userImage)
{

  QString htmlDocument;

  QString divStart = "<div class=\"container darker\">";
  QString divEnd = "<div class=\"container darker\">";

  // the user has uploaded an image
  QString imageTag = "<img src=\"";
  imageTag += userImage;
  imageTag += "\" class=\"right\">";
  
  QString nameTag = "<span><strong>";
  nameTag += username;
  nameTag += ":</strong></span>";

  QString messageTag = "<p>";
  messageTag = message;
  messageTag = "</p>";

  QString timeTag = "<span class=\"time-right\">";
  timeTag += "11:00";
  timeTag += "</span>";

  htmlDocument+=divStart;
  htmlDocument+=imageTag;
  htmlDocument+=nameTag;
  htmlDocument+=messageTag;
  htmlDocument+=timeTag;
  htmlDocument+=divEnd;
  
  //send message
  m_chatClient->sendMessage(htmlDocument);
}



void ChatWindow::buildLocalHtmlTags(const QString& userName,const QString& message, const QString& userImage)
{


 QString htmlStart = "<!DOCTYPE html>\n<html>\n<head>\n<link href=\"chatStyle.css\" rel=\"stylesheet\" ctype=\"text/css\">\n</head>\n<body>";

m_htmlDoc.push_back(htmlStart);
  
  QString divEnd = "</div>";
    QString divStart = "<div class=\"container\">";

  m_htmlDoc.push_back(divStart);
  // the user has uploaded an image
  QString imageTag = "<img src=\"";
  imageTag += userImage;
  imageTag += "\">";

  m_htmlDoc.push_back(imageTag);

  QString nameTag = "<span><strong>";
  nameTag += userName;
  nameTag += ":</strong></span>";
  
  m_htmlDoc.push_back( nameTag);

  QString messageTag = "<p>";
  messageTag += message;
  messageTag += "</p>";
  
   m_htmlDoc.push_back( messageTag);
  

  QString timeTag = "<span class=\"time-left\">";
  timeTag += "11:00";
  timeTag += "</span>";
  m_htmlDoc.push_back(timeTag);

  m_htmlDoc.push_back(divEnd);
  QString htmlbodyEnd = "</body>";
  QString htmlEnd = "</html>";
  m_htmlDoc.push_back(htmlbodyEnd);
  m_htmlDoc.push_back(htmlEnd);

}

void ChatWindow::sendMessage()
{
    // TODO:for now we hardcoded to test if this approachi will work
    QString message = ui->lineEdit->text();
    ui->lineEdit->setText("");
    QString userImage = "somefile.jpg";

    buildHtmlSendTags("paco",message, userImage);
    
      
    //build localHtml
    buildLocalHtmlTags("paco",message, userImage);
    
      
     loadHtmlConversation();
      
}