#include "ui_loginwindow.h"
#include "chatwindow.h"
#include "loginwindow.h"
#include "SignUpWindow.h"
#include <QStandardItemModel>
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
    , m_chatWindow(new ChatWindow(this))
    , m_signUpWindow(new SignUpWindow(this))
    , m_chatWindowIndex{0}
    , m_singUpIndex{0}
{
    ui->setupUi(this);

    // Add GUIs to the stackedWidget and get the index locations
    m_chatWindowIndex = ui->stackedWidget->addWidget(m_chatWindow);
    m_singUpIndex = ui->stackedWidget->addWidget(m_signUpWindow);

    //connect signals and slots
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(initLoggin()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(initSingUp()));
    connect(this, SIGNAL(connectToServer(const QString &, const QString &)), m_chatWindow, SLOT(connectToServer(const QString &, const QString &)));
    connect(m_chatWindow, SIGNAL(userVerified()), this, SLOT(onUserVerified()));
    connect(m_chatWindow, SIGNAL(userVerifyError(const QString &)), this, SLOT(showError(const QString &)));
}
LoginWindow::~LoginWindow()
{
    delete ui;
}
void LoginWindow::onUserVerified()
{
    //On succes open chat Window
    ui->stackedWidget->setCurrentIndex(m_chatWindowIndex);
}

void LoginWindow::initLoggin()
{
    QString userName = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();
    emit connectToServer(userName, password);
}

void LoginWindow::initSingUp()
{
    ui->stackedWidget->setCurrentIndex(m_singUpIndex);
}
void LoginWindow::changeToLogInWindow()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void LoginWindow::showError(const QString &reason)
{
    QMessageBox::information(this, "Error Log in up", reason);
}
