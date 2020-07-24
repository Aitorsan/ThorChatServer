#include "ui_loginwindow.h"
#include "chatwindow.h"
#include "loginwindow.h"
#include "SignUpWindow.h"
#include <QStandardItemModel>


LoginWindow::LoginWindow(QWidget* parent)
    :QMainWindow(parent)
    ,ui(new Ui::LoginWindow)
    ,m_chatModel(new QStandardItemModel(this)) 
    ,m_chatWindow(new ChatWindow(*m_chatModel,nullptr))
    ,m_signUpWindow( new SignUpWindow(this))
    ,m_chatWindowIndex{0}
    ,m_singUpIndex{0}
{
    ui->setupUi(this);
   
   // Add GUIs to the stackedWidget and get the index locations
    m_chatWindowIndex = ui->stackedWidget->addWidget(m_chatWindow);
    m_singUpIndex = ui->stackedWidget->addWidget(m_signUpWindow);

    //connect signals and slots
    connect(ui->pushButton, SIGNAL(clicked()),this,SLOT(initLoggin()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(initSingUp()));
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

void LoginWindow::initSingUp()
{

    ui->stackedWidget->setCurrentIndex(m_singUpIndex);

}
LoginWindow::~LoginWindow()
{
    delete ui;
}