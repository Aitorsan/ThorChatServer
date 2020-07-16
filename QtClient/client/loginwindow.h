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
signals:
    void verifyUser();
private:
    Ui::LoginWindow* ui;
    QStandardItemModel *m_chatModel;
    ChatWindow* m_chatWindow;
    int m_chatWindowIndex;
};


#endif //LOGGING_WINDOW_H