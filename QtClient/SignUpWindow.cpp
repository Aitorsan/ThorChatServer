
#include "ui_signupwindow.h"
#include "SignUpWindow.h"



SignUpWindow::SignUpWindow(QWidget* parent)
    :QMainWindow(parent)
    ,ui(new Ui::SignUpWindow)
{
    ui->setupUi(this);
}


SignUpWindow::~SignUpWindow()
{
    
}
