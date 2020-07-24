#include "CustomTextEdit.h"
#include <QKeyEvent>

            
CustomTextEdit::CustomTextEdit(QWidget* parent)
    :QTextEdit(parent)
    , m_plainText{}
{

    //initialize all the stuff

   
}



void CustomTextEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return)
    {    
        m_plainText = toPlainText();
        emit returnPressed();
    }
    else
        QTextEdit::keyPressEvent(event);


}

void CustomTextEdit::clearText()
{
    m_plainText = "";
    clear();
}
