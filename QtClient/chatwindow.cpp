#include "ui_chatwindow.h"
#include "chatwindow.h"
#include "CustomTextEdit.h"
#include "messageData.h"
#include "../src/include/base64.h"
#include <QString>
#include <QHostAddress>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QFile>
#include <QDebug>
#include <QUrl>
#include <QWebFrame>
#include <QIODevice>
#include <QDir>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QList>
#include <QDebug>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <string>

ChatWindow::ChatWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ChatWindow)
    , m_customTextEdit(new CustomTextEdit())
    , m_chatModel(new QStandardItemModel(this))
    , m_chatClient(new ChatClient(this))
    , m_userName{}
    , m_password{}
    , m_image{}
    , m_htmlDocList{}
    , m_stylesDir{"file://"+QDir::currentPath()+ "/res/styles/chatStyle.css"}
{
  ui->setupUi(this);

  // create and custom widgets to the ui
  ui->verticalLayout->addWidget(m_customTextEdit);

  // the model for the messages will have 1 column
  m_chatModel->insertColumn(0);
  //set the model for the list view
  ui->listView->setModel(m_chatModel);

  // prepare the html document
  QString htmlStart = "<!DOCTYPE html>\n<html>\n<head>\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n<link href=\"chatStyle.css\" rel=\"stylesheet\" ctype=\"text/css\">\n</head>\n<body>";
  m_htmlDocList.push_back(htmlStart);
  QString htmlbodyEnd = "</body>";
  QString htmlEnd = "</html>";
  m_htmlDocList.push_back(htmlbodyEnd);
  m_htmlDocList.push_back(htmlEnd); 

  // connect signals and slots
  connect(m_chatClient, SIGNAL(messageReceived(const MessageData&)), this, SLOT(msgReceived(const MessageData&)));
  connect(m_chatClient,SIGNAL(loggedIn()),this,SIGNAL(userVerified()));
  connect(m_chatClient,SIGNAL(loggedIn()),this,SLOT(loadProfileImage()));
  connect(m_chatClient,SIGNAL(userJoined(const MessageData &)),this,SLOT(addUserToListView(const MessageData&)));
  connect(m_chatClient,SIGNAL(userLeft(const MessageData&)),this,SLOT(removeUserFromListView(const MessageData& )));
  connect(m_customTextEdit, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
  connect(m_chatClient, SIGNAL(sendLogInData()), this, SLOT(sendLogInData()));
  connect(m_chatClient,SIGNAL(loginError(const QString&)), this, SIGNAL(userVerifyError(const QString&)));

}

ChatWindow::~ChatWindow()
{
  delete ui;
}

void ChatWindow::loadProfileImage()
{
  //read the image file 

   std::stringstream os;
   std::string imageName {"64_"};
   
   // compute the time
    auto time = (unsigned int)std::time(nullptr);
    auto randomImage = time%17;
    
    imageName += std::to_string(randomImage);
    imageName +=".png";
    qDebug() << QString::fromStdString(imageName);
    QString file = QDir::currentPath()+ "/res/icons/"+QString::fromStdString(imageName);
   os << std::ifstream(file.toStdString(), std::ios::binary).rdbuf();
   //Encode the image in base64 and asssign the image to the user image

   std::string imdata = base64(os.str());
   m_image = QString::fromStdString(imdata);

   qDebug() << "READ IMAGE:" << m_image;

}


void ChatWindow::sendLogInData()
{

  MessageData loginData;
  loginData.m_sender = m_userName;
  loginData.m_text = m_password;
  
  //create the json message
  QString temp = m_chatClient->formatMessage(loginData,MsgType::LOGIN);
  
  m_chatClient->sendMessage(temp);
}

void ChatWindow::msgReceived(const MessageData& msg)
{

  QString htmlDoc = buildHtmlRecievedMsgTags(msg);

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

QString ChatWindow::buildHtmlRecievedMsgTags(const MessageData &msg)
{
  QString divStart = "<div class=\"container darker\">";
  QString divEnd = "</div>";

  m_htmlDocList.push_back(divStart);

  // a bit expensive comparaton
  if( msg.m_base64Image != "none")
  {
    QString imageTag = "<img src=\"data:image/";
    imageTag += msg.m_imageExtension;
    imageTag += ";";
    imageTag += "base64,";
    imageTag += msg.m_base64Image;
    imageTag += "\" class=\"right\">";
    m_htmlDocList.push_back(imageTag);
  }

  QString nameTag = "<span><strong>";
  nameTag += msg.m_sender;
  nameTag += ":</strong></span>";

  m_htmlDocList.push_back(nameTag); 

  QString messageTag = "<p>";
  messageTag += msg.m_text;
  messageTag += "</p>";

  m_htmlDocList.push_back(messageTag);

  QString timeTag = "<span class=\"time-right\">";
  timeTag += msg.m_time;
  timeTag += "</span>";
  m_htmlDocList.push_back(timeTag);

  m_htmlDocList.push_back(divEnd);

  QString htmlDocument;
  for(auto text : m_htmlDocList)
    htmlDocument += text;

  return htmlDocument;
}

QString ChatWindow::buildLocalHtmlTags(const MessageData& localMsg)
{
  QString divEnd = "</div>";
  QString divStart = "<div class=\"container\">";

  m_htmlDocList.push_back(divStart);

  QString imageTag = "<img src=\"data:image/";
  imageTag += localMsg.m_imageExtension;
  imageTag += ";";
  imageTag += "base64,";
  imageTag += localMsg.m_base64Image;
  imageTag += "\" class=\"left\">";

  qDebug() << imageTag;
  m_htmlDocList.push_back(imageTag);

  QString nameTag = "<span><strong>";
  nameTag += localMsg.m_sender;
  nameTag += ":</strong></span>";

  m_htmlDocList.push_back(nameTag);

  QString messageTag = "<p>";
  messageTag += localMsg.m_text;
  messageTag += "</p>";

  m_htmlDocList.push_back(messageTag);

  QString timeTag = "<span class=\"time-left\">";
  timeTag += localMsg.m_time;
  timeTag += "</span>";
  m_htmlDocList.push_back(timeTag);

  m_htmlDocList.push_back(divEnd);

  QString htmldocument;

  for(auto text : m_htmlDocList)
    htmldocument += text;
  
  return htmldocument;
}

void ChatWindow::sendMessage()
{
  // Get the message from the line edit
  QString message = m_customTextEdit->toPlainText();
  m_customTextEdit->clearText();//clear the line edit
  
  MessageData msg;
  msg.m_sender = m_userName;
  msg.m_text = message;
  msg.m_base64Image = m_image;
  msg.m_imageExtension ="png";
  QString formattedMsg = m_chatClient->formatMessage(msg,MsgType::MESSAGE);

  //build localHtml
  QString localDocument = buildLocalHtmlTags(msg);
  loadHtmlConversation(localDocument);
  
  //send message
  m_chatClient->sendMessage(formattedMsg);
}

void ChatWindow::addUserToListView(const MessageData& newUserLogged)
{
  // store the index of the new row to append to the model containing the users
  int newRow = m_chatModel->rowCount();

  // insert row for the username
  m_chatModel->insertRow(newRow);

  // store the username in the model
  m_chatModel->setData(m_chatModel->index(newRow,0), newUserLogged.m_sender);
  
  // scroll the view to display the new user
  ui->listView->scrollToBottom();
  
}

void ChatWindow::removeUserFromListView(const MessageData& userLeft)
{
  // find he user in the list
  QList<QStandardItem*> items = m_chatModel->findItems(userLeft.m_sender);
  
  /*TODO: Check all and compare IDs,it could be that 2 users have the same name
    for now we asume that it will be just one user with the same name until we
    sort out the database system in the server
  */
  if( !items.isEmpty())
  {
    const QStandardItem& item = *(items.first());
    int row = item.row();
    m_chatModel->removeRow(row); 
  }
  
}
