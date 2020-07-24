#include "ui_chatwindow.h"
#include "chatwindow.h"
#include "CustomTextEdit.h"
#include <QString>
#include <QHostAddress>
#include <QStandardItemModel>
#include <QFile>
#include <QDebug>
#include <QUrl>
#include <QWebFrame>
#include <QIODevice>
#include <fstream>
#include <QDir>
#include <chrono>
#include <sstream>
#include <iomanip>

ChatWindow::ChatWindow(QStandardItemModel &model, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatWindow)
    , m_customTextEdit(new CustomTextEdit())
    , m_chatModel(model)
    , m_chatClient(new ChatClient(this))
    , m_userName{}
    , m_password{}
    , m_htmlDocList{}
    , m_stylesDir{"file://"+QDir::currentPath()+ "/res/styles/chatStyle.css"}
{
  ui->setupUi(this);


  // create and custom widgets to the ui
  ui->verticalLayout->addWidget(m_customTextEdit);

  // prepare the html document
  QString htmlStart = "<!DOCTYPE html>\n<html>\n<head>\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n<link href=\"chatStyle.css\" rel=\"stylesheet\" ctype=\"text/css\">\n</head>\n<body>";
  m_htmlDocList.push_back(htmlStart);
  QString htmlbodyEnd = "</body>";
  QString htmlEnd = "</html>";
  m_htmlDocList.push_back(htmlbodyEnd);
  m_htmlDocList.push_back(htmlEnd); 


  // connect signals and slots
  connect(m_chatClient, SIGNAL(messageReceived(const QString&,const QString&,const QString &,const QString&)), this, SLOT(msgReceived(const QString&,const QString &,const QString&, const QString&)));
  connect(m_chatClient,SIGNAL(loggedIn()),this,SIGNAL(userVerified()));

  connect(m_customTextEdit, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
  connect(m_chatClient, &ChatClient::sendLogInData, this, &ChatWindow::sendLogInData);
  //connect(ui->lineEdit,SIGNAL(),this)
  /* connect(QWidget *centralwidget;
   connect(*actionSave_history;
    connect(QAction *actiondelete_history;
   connect( QGridLayout *gridLayout;
   connect( QVBoxLayout *verticalLayout;
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

void ChatWindow::sendLogInData()
{

  auto time = std::time(nullptr);
  std::stringstream buff;
  buff << std::put_time(std::gmtime(&time), "%d/%m/%y %T%z");
  QString sentTime = QString::fromStdString(buff.str());

  QString temp = m_chatClient->formatLogginMessage(m_userName,m_password, sentTime);
  m_chatClient->sendMessage(temp);
}

void ChatWindow::msgReceived(const QString& sender,const QString &msg, const QString &time, const QString& image)
{

  qDebug() << "msgREceived:" << msg;

  QString htmlDoc = buildHtmlRecievedMsgTags(sender,msg,time,image);

  loadHtmlConversation(htmlDoc);

}


void ChatWindow::loadHtmlConversation(const QString& htmlDoc)
{
   QString html = htmlDoc;
   html += "</body>";
   html+= "</html>";

  ui->webView->setHtml(html,QUrl(m_stylesDir));
  ui->webView->page()->mainFrame()->evaluateJavaScript("window.scrollTo(0, document.body.scrollHeight);");
}


void ChatWindow::connectToServer(const QString &username, const QString &password)
{
  m_userName = username;
  m_password = password;
  m_chatClient->connectToServer(QHostAddress("0.0.0.0"), 55555);

}

QString ChatWindow::buildHtmlRecievedMsgTags(const QString &username, const QString &message,const QString& time, const QString &userImage)
{
 
  QString divStart = "<div class=\"container darker\">";
  QString divEnd = "</div>";

  m_htmlDocList.push_back(divStart);
  // the user has uploaded an image
  // the user has uploaded an image
  QString imageTag = "<img src=\"";
  imageTag += userImage;
  imageTag += "\" class=\"right\">";
  m_htmlDocList.push_back(imageTag);

  QString nameTag = "<span><strong>";
  nameTag += username;
  nameTag += ":</strong></span>";

  m_htmlDocList.push_back(nameTag); 

  QString messageTag = "<p>";
  messageTag += message;
  messageTag += "</p>";

  m_htmlDocList.push_back(messageTag);

  QString timeTag = "<span class=\"time-right\">";
  timeTag += time;
  timeTag += "</span>";
  m_htmlDocList.push_back(timeTag);

  m_htmlDocList.push_back(divEnd);

  QString htmlDocument;
  for(auto text : m_htmlDocList)
    htmlDocument+= text;

  return htmlDocument;
}

QString ChatWindow::buildLocalHtmlTags(const QString &userName, const QString& message,const QString &time, const QString &userImage)
{

  QString divEnd = "</div>";
  QString divStart = "<div class=\"container\">";

  m_htmlDocList.push_back(divStart);
  // the user has uploaded an image
  QString imageTag = "<img src=\"";
  imageTag += userImage;
  imageTag += "\">";

  m_htmlDocList.push_back(imageTag);

  QString nameTag = "<span><strong>";
  nameTag += userName;
  nameTag += ":</strong></span>";

  m_htmlDocList.push_back(nameTag);

  QString messageTag = "<p>";
  messageTag += message;
  messageTag += "</p>";

  m_htmlDocList.push_back(messageTag);

  QString timeTag = "<span class=\"time-left\">";
  timeTag += time;
  timeTag += "</span>";
  m_htmlDocList.push_back(timeTag);

  m_htmlDocList.push_back(divEnd);

  QString htmldocument;
  for(auto text : m_htmlDocList)
    htmldocument+= text;
  

   return htmldocument;
}

void ChatWindow::sendMessage()
{
  // Get the message from the line edit
  QString message = m_customTextEdit->toPlainText();
  m_customTextEdit->clearText();//clear the line edit
  auto time = std::time(nullptr);
  std::stringstream buff;
  buff << std::put_time(std::gmtime(&time), "%d/%m/%y %T%z");
  QString sentTime = QString::fromStdString(buff.str());
 
  //build localHtml
  QString localDocument = buildLocalHtmlTags(m_userName, message,sentTime, "userImage");

  loadHtmlConversation(localDocument);

  //send message
  QString formattedMsg = m_chatClient->formatMessage(m_userName,sentTime,message);

  m_chatClient->sendMessage(formattedMsg);
}