#include "ui_loginwindow.h"
#include "chatwindow.h"
#include "loginwindow.h"
#include <QStandardItemModel>


LoginWindow::LoginWindow(QWidget* parent)
    :QMainWindow(parent)
    ,ui(new Ui::LoginWindow)
    ,m_chatModel(new QStandardItemModel(this)) 
    ,m_chatWindow(new ChatWindow(*m_chatModel,nullptr))
    ,m_chatWindowIndex{2}
{
    ui->setupUi(this);

    m_chatWindowIndex = ui->stackedWidget->addWidget(m_chatWindow);

    connect(ui->pushButton, SIGNAL(clicked()),this,SLOT(initLoggin()));
    connect( this, SIGNAL(connectToServer(const QString& ,const QString&)), m_chatWindow,SLOT(connectToServer(const QString& , const QString& )));
    connect(m_chatWindow,SIGNAL(userVerified()), this, SLOT(onUserVerified()));
}

void LoginWindow::onUserVerified()
{
   // Verify user for that I need to connect to the server
  ui->stackedWidget->setCurrentIndex(m_chatWindowIndex);
}

void LoginWindow::initLoggin()
{
   QString userName =  ui->lineEdit->text();
   QString password =  ui->lineEdit_2->text();
   
   emit connectToServer(userName,password);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}