#ifndef CUSTOM_TEXT_EDIT_H
#define CUSTOM_TEXT_EDIT_H

#include <QTextEdit>
#include <QString>


class QKeyEvent;

class CustomTextEdit: public QTextEdit
{ 
     Q_OBJECT
public:
    explicit CustomTextEdit(QWidget* parent = nullptr);
    void keyPressEvent(QKeyEvent* event) override;
    QString getPlainText() const;
    void clearText();
signals:
   void returnPressed();
private:
   QString m_plainText;
};

#endif