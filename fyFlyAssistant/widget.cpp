#include "widget.h"
#include "ui_widget.h"

#include <QIcon>
#include <QPushButton>
#include <QResizeEvent>
#include <QTabWidget>


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

void Widget::settingButton_clicked_slot()
{
    if(true == mSettingWindow->isHidden())
    {
        mSettingWindow->windowShow();
    }
    else
    {
        mSettingWindow->windowHide();
    }
}

void Widget::flyBoardDataShowButton_clicked_slot()
{
    if(true == mflyBoardDataShowWidget->isHidden())
    {
        mflyBoardDataShowWidget->windowShow();
    }
    else
    {
        mflyBoardDataShowWidget->windowHide();
    }
}

void Widget::showLogMessage_slot(QByteArray logMessage)
{
    logTextEdit->append(logMessage);
}

void Widget::windowInit()
{
    this->setWindowTitle("fyFlyAssistant");     /* 设置窗口名称 */
    this->resize(800,600);
    this->setMinimumSize(800, 600);             /* 设置窗口最小尺寸 */

    /*设置按钮 */
    settingButton = new QPushButton(this);
    QIcon settingButtonIcon("://images/button/settingButton.png");
    settingButton->setIconSize(QSize(32,32));
    settingButton->move(800-32,600-32);
    settingButton->setIcon(settingButtonIcon);
    settingButton->setStyleSheet( "QPushButton{border:0px;background:white;}"
                                  "QPushButton:hover{border:0px;background:blue;}"
                                  "QPushButton:pressed{border:0px;background:red;}");

    /* 飞控数据显示按钮 */
    flyBoardDataShowButton = new QPushButton(this);
    QIcon flyBoardDataShowIcon("://images/button/flyBoardDataButton.png");
    flyBoardDataShowButton->setIconSize(QSize(32,32));
    flyBoardDataShowButton->move(800-64,600-32);
    flyBoardDataShowButton->setIcon(flyBoardDataShowIcon);
    flyBoardDataShowButton->setStyleSheet( "QPushButton{border:0px;background:white;}"
                                  "QPushButton:hover{border:0px;background:blue;}"
                                  "QPushButton:pressed{border:0px;background:red;}");


    /* 新建设置窗口 */
    settingWindow = new QWidget(this);
    mSettingWindow = new settingWidget(settingWindow);
    mSettingWindow->setWindowSize(800,300);

    /* 新建飞控数据显示窗口 */
    flyDataShowWindow = new QWidget(this);
    mflyBoardDataShowWidget = new flyBoardDataShowWidget(flyDataShowWindow);
    mflyBoardDataShowWidget->setWindowSize(800,300);

    /* 创建QTextEdit对象 */
    logTextEdit = new QTextEdit(this);
    logTextEdit->setGeometry(0, 300, 200, 100); /* 设置QTextEdit的位置和大小 */
    logTextEdit->show();                        /* 将QTextEdit添加到主窗口 */

    connect(this,&Widget::appLogMessage_signal,this,&Widget::showLogMessage_slot);                      /* 连接log信息打印的信号,信号发出者:Widget */
    connect(mSettingWindow,&settingWidget::appLogMessage_signal,this,&Widget::showLogMessage_slot);     /* 连接log信息打印的信号,信号发出者:mSettingWindow */

    connect(settingButton, &QPushButton::clicked,this,&Widget::settingButton_clicked_slot);             /* 连接设置按键的单击信号和槽 */
    connect(flyBoardDataShowButton, &QPushButton::clicked,this,&Widget::flyBoardDataShowButton_clicked_slot);             /* 连接飞控数据显示按键的单击信号和槽 */
}

void Widget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    settingButton->move(this->width()-32,this->height()-32);
    flyBoardDataShowButton->move(this->width()-64,this->height()-32);

    mSettingWindow->setWindowSize(this->width(),300);
    mflyBoardDataShowWidget->setWindowSize(this->width(),300);
}

bool Widget::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);
    MSG* msg = reinterpret_cast<MSG*>(message);
       if(msg->message == WM_DEVICECHANGE)                // 通知应用程序设备或计算机的硬件配置发生更改。
       {
           PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
           switch (msg->wParam)
           {
           case DBT_DEVICEARRIVAL:             // 插入
           {
               if (lpdb->dbch_devicetype == DBT_DEVTYP_PORT)           // 设备类型为串口
               {
                   if(false == mSettingWindow->serialPortConnectState)   /* 处于连接状态 */
                   {
                       emit appLogMessage_signal("检测有设备插入");
                       mSettingWindow->searchSerialPort();/*更新串口*/
                   }
               }
               break;
           }
           case DBT_DEVICEREMOVECOMPLETE:      // 拔出
           {
               if (lpdb->dbch_devicetype == DBT_DEVTYP_PORT)           // 设备类型为串口
               {
                   PDEV_BROADCAST_PORT lpdbv = (PDEV_BROADCAST_PORT)lpdb;
                   QString strName = QString::fromWCharArray(lpdbv->dbcp_name);  //拔出的串口名
                   if(mSettingWindow->openedSerialPortName == strName &&
                           true == mSettingWindow->serialPortConnectState)      /* 连接状态下，对应连接的串口弹出 */
                   {
                       mSettingWindow->closeSerialPort();   /* 关闭打开的串口 */
                       emit appLogMessage_signal("检测有设备拔出");
                   }
                   mSettingWindow->searchSerialPort();/*更新串口*/
               }
               break;
           }
           default:
               break;
           }
       }
       return false;
}
