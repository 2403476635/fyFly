#ifndef MYCOMBOBOX_H
#define MYCOMBOBOX_H

#include <QObject>
#include <QComboBox>        //导入头文件，如果你连基类的头文件都不导入，继承谁啊
#include <QMouseEvent>      //导入鼠标事件的头文件，你没有这个头文件，你点鼠标人家也不给你干活的
#include <QSerialPortInfo>

class myComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit myComboBox(QWidget *parent = nullptr);

protected:
    virtual void mousePressEvent(QMouseEvent *event);  //重写鼠标点击事件
signals:
    void clicked_signals();

};

#endif // MYCOMBOBOX_H
