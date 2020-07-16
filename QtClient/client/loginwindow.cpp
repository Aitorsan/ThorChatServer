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

    connect(ui->pushButton, SIGNAL(clicked()),m_chatWindow,SLOT(connectToServer()));
    connect(m_chatWindow,SIGNAL(userVerified()), this, SLOT(onUserVerified()));
}


void LoginWindow::onUserVerified()
{
   // Verify user for that I need to connect to the server
  ui->stackedWidget->setCurrentIndex(m_chatWindowIndex);
}


LoginWindow::~LoginWindow()
{
    delete ui;
}