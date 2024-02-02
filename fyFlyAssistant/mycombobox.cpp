#include "mycombobox.h"
myComboBox::myComboBox(QWidget *parent)
    : QComboBox{parent}
{

}

void myComboBox::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        emit clicked_signals();             /* 是的话就发送咱们定义的信号 */
    }
    QComboBox::mousePressEvent(event);      /* 如果不写这一句，事件传递到上一步就停止，就不接着向下发了，父类也就没办法处理点击事件了 */
}
