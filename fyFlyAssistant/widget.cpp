#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    windowInit();/* 窗口初始化 */
}

Widget::~Widget()
{
    delete ui;
}

void Widget::windowInit()
{
    this->setWindowTitle("fyFlyAssistant");
}

