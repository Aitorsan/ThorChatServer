
#include "ui_signupwindow.h"
#include "SignUpWindow.h"
#include <QString>
#include <QDebug>
#include <QMessageBox>

SignUpWindow::SignUpWindow(QWidget* parent)
    :QMainWindow(parent)
    ,ui(new Ui::SignUpWindow)
{
    ui->setupUi(this);

    //connect signals and slots
    connect(ui->pushButton_3,SIGNAL(clicked()), parent,SLOT(changeToLogInWindow()));
    connect(ui->pushButton_3,SIGNAL(clicked()), this,SLOT(restoreTextState()));
    connect(ui->pushButton,SIGNAL(clicked()), this,SLOT(singUpConnection()));
    connect(this,SIGNAL(connectToServer(const QString&,const QString&)),parent,SIGNAL(connectToServer(const QString&,const QString&)));
}

SignUpWindow::~SignUpWindow()
{
    
}
void SignUpWindow::restoreTextState()
{
   ui->plainTextEdit->clear();
   ui->textEdit->clear();
}


void SignUpWindow::singUpConnection()
{
    QString username = ui->plainTextEdit->document()->toPlainText();
    QString password = ui->textEdit->document()->toPlainText();
    qDebug() << "username:" << username;
    qDebug() << "passsword:" << password;

    if (username.isEmpty() || password.isEmpty())
        QMessageBox::information(this, "Error Sing up","Invalid empty username or password" );
   // else
      //emit connectToServer(username,password);
}
