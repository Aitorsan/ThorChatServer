

#ifndef SINGUP_WINDOW_H
#define SINGUP_WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class SignUpWindow;}
QT_END_NAMESPACE


class SignUpWindow:public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(SignUpWindow)

public:
	explicit SignUpWindow(QWidget *parent = nullptr);
    ~SignUpWindow();

signals:
    void connectToServer(const QString&,const QString&);

public slots:
    void singUpConnection();
    void restoreTextState();

private:

	Ui::SignUpWindow* ui;

};
#endif
		
