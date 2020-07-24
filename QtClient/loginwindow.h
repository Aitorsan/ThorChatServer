#ifndef LOGGINWINDOW_H
#define LOGGINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class LoginWindow;
}
QT_END_NAMESPACE

class ChatWindow;
class SignUpWindow;
class QStandardItemModel;

class LoginWindow : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(LoginWindow)
public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();
public slots:
    void onUserVerified();
    void initLoggin();
    void initSingUp();

signals:
    void verifyUser();
    void connectToServer(const QString& ,const QString&);
private:
    Ui::LoginWindow* ui;
    QStandardItemModel *m_chatModel;
    ChatWindow* m_chatWindow;
    SignUpWindow* m_signUpWindow;
    int m_chatWindowIndex;
    int m_singUpIndex;
};


#endif //LOGGING_WINDOW_H