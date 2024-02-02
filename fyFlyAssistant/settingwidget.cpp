#include "settingwidget.h"

settingWidget::settingWidget(QWidget *parent)
    : QWidget{parent}
{
    settingWindow = parent;
    settingWindow->setStyleSheet("background-color: red;");
    settingWindow->setWindowFlags(Qt::FramelessWindowHint); /* 设置窗口为无边框 */
    settingWindow->hide();

    /* 加载显示的图标 */
    waitOpenSerialPort.addFile("://images/state/waitConnetct.png");
    openSerialPortSuccess.addFile("://images/state/connectSuccess.png");

    /* 创建settingTabWidget */
    settingTabWidget = new QTabWidget(settingWindow);
    settingTabWidget->setTabShape(QTabWidget::Rounded);
    settingTabWidget->setTabPosition(QTabWidget::South);
    /* 创建connectSelectPageWidget连接设置界面 */
    QWidget *connectSelectPageWidget = new QWidget(settingWindow);

    connectSelectPageWidget->setStyleSheet("background-color: lightblue;"); /* 设置背景颜色，测试用 */

    settingTabWidget->addTab(connectSelectPageWidget, "连接设置");
//    tabWidget->setStyleSheet(   "QTabWidget::tab-bar {"
//                                "    alignment: center;"
//                                "}"
//                                "QTabBar::tab {"
//                                "    background-color: lightgray;"
//                                "    border: 1px solid gray;"
//                                "    border-bottom-color: white;"
//                                "    padding: 8px;"
//                                "}"
//                                "QTabBar::tab:selected {"
//                                "    background-color: white;"
//                                "    border-bottom-color: white;"
//                                "}"
//                                "QTabWidget::pane {"
//                                "    border: 1px solid gray;"
//                                "}");
    settingTabWidget->setStyleSheet(
                "QTabBar::tab {"
                "width: 200px;"
                "}"
                "QTabWidget::tab-bar {"
                               "    alignment: center;"
                               "}");
    settingTabWidget->setUsesScrollButtons(false);         /* 关闭页面切换滚轮 */
    settingTabWidget->resize(600, 300);                    /* 设置控件大小 */
    settingTabWidget->show();                              /* 显示 */

    /* 创建串口连接QGroupBox对象 */
    serialPortgroupBox = new QGroupBox("串口连接", connectSelectPageWidget);
    serialPortgroupBox->setGeometry(10, 10, 300-10, 300-40);/* 设置QGroupBox的位置和大小 */
    serialPortgroupBox->show();/* 将QGroupBox添加到主窗口 */

    openSerialPortPushButton = new QPushButton(serialPortgroupBox);                    /* 打开串口按键 */

    openSerialPortPushButton->setStyleSheet( "QPushButton{border:0px;background:white;}"
                                             "QPushButton:hover{border:0px;background:blue;}"
                                             "QPushButton:pressed{border:0px;background:red;}");
    openSerialPortPushButton->setText("打开串口");
    openSerialPortPushButton->setIcon(waitOpenSerialPort);  /* 加载等待连接的图标 */

    /* 创建显示串口的combox */
    serialPortComboBox = new myComboBox(serialPortgroupBox);
    /* 创建设置串口波特率的输入框 */
    serialPortRateLineEdit = new  QLineEdit(serialPortgroupBox);
    serialPortRateLineEdit->setText("115200");
    serialPortRateLineEdit->setAlignment(Qt::AlignCenter); /* 将QLineEdit的文本居中显示 */

    serialPortComboBox->resize(120,20);
    serialPortComboBox->move(10,20);
    serialPortRateLineEdit->setGeometry(10,40, 120, 20); /* 设置QLineEdit的位置和大小 */
    openSerialPortPushButton->resize(120,20);
    openSerialPortPushButton->move(10,60);


    connect(openSerialPortPushButton, &QPushButton::clicked,this,&settingWidget::openSerialPortPushButton_clicked_slot);   /* 连接打开串口响应的单击信号和槽 */

    connect(serialPortComboBox,&myComboBox::clicked_signals,this,[=](){searchSerialPort();});

    searchSerialPort();
}

settingWidget::~settingWidget()
{

}

void settingWidget::setWindowSize(int width, int height)
{
    settingWindow->resize(width, height);
}

void settingWidget::windowShow()
{
    settingWindow->show();
}

void settingWidget::windowHide()
{
    settingWindow->hide();
}

bool settingWidget::isHidden() const
{
    return settingWindow->isHidden();
}

void settingWidget::openSerialPortPushButton_clicked_slot()
{
    if(openSerialPortPushButton->text() == "打开串口")
    {
        QString portName = serialPortComboBox->currentText();
        int PortBandRate = serialPortRateLineEdit->text().toInt();
        QSerialPort::DataBits PortDataBits = QSerialPort::Data8;                /*数据位，默认Data8*/
        QSerialPort::Parity PortParityBits = QSerialPort::NoParity;             /*校验位，默认NoParity*/
        QSerialPort::StopBits PortStopBits = QSerialPort::OneStop;              /*停止位，默认OneStop*/
        QSerialPort::FlowControl PortFlowControl = QSerialPort::NoFlowControl;  /*串口控制，默认NoFlowControl*/

        if(portName.isEmpty())
        {
            emit appLogMessage_signal("选择的端口号为空");
            return ;
        }

        serialPortThread = new QThread;
        serialPortTask = new SerialPortThread(portName,PortBandRate,PortDataBits,PortParityBits,PortStopBits,PortFlowControl);
        serialPortTask->moveToThread(serialPortThread);

        connect(this,&settingWidget::closeSerialPort_signal,serialPortTask,&SerialPortThread::SerialPortClose);           /* 连接串口关闭处理槽函数 */
        connect(this,&settingWidget::sendSerialPortData,serialPortTask,&SerialPortThread::DataSend);                      /* 连接串口发送数据的槽函数 */
        connect(serialPortTask,&SerialPortThread::appLogMessage_signal,this,&settingWidget::appLogMessage_signal);        /* 子线程输出log信息到主线程 */
//        connect(this,&Widget::sendBinFileData_signal,serialPortTask,&SerialPortThread::sendBinFile,Qt::UniqueConnection);               /* 发送bin文件 */

        connect(serialPortTask,&SerialPortThread::OpenSerialPortOK_signal,this,&settingWidget::openSerialPortState_slot);  /* 连接串口设备连接状态的处理槽函数 */

        connect(serialPortThread, &QThread::started, serialPortTask,&SerialPortThread::openSerialPort);                    /* 连接线程启动后的槽函数 */

        serialPortThread->start();
    }
    else if(openSerialPortPushButton->text() == "关闭串口")
    {
        serialPortTask->SerialPortClose();
        serialPortComboBox->clear();
        serialPortComboBox->setDisabled(false);

        serialPortThread->exit(0);
        serialPortThread->wait();
        serialPortThread->deleteLater();

        serialPortTask->deleteLater();

        openSerialPortPushButton->setIcon(waitOpenSerialPort);      /* 加载等待连接的图标 */
        openSerialPortPushButton->setText("打开串口");

        serialPortConnectState = false;

        emit appLogMessage_signal("串口已关闭");
    }
}

void settingWidget::openSerialPortState_slot(bool state)
{
    if(state)   /* 打开串口成功 */
    {
        openSerialPortPushButton->setIcon(openSerialPortSuccess);   /* 加载成功连接的图标 */
        openSerialPortPushButton->setText("关闭串口");
        serialPortComboBox->setDisabled(true);

        serialPortConnectState = true;
        openedSerialPortName = serialPortComboBox->currentText();   /* 获取当前打开的串口端口号 */

        emit appLogMessage_signal("设备连接成功");
    }
    else        /* 打开串口失败 */
    {
        openSerialPortPushButton->setIcon(waitOpenSerialPort);      /* 加载等待连接的图标 */
        serialPortThread->exit(0);
        serialPortThread->wait();
        serialPortThread->deleteLater();

        serialPortConnectState = false;

        serialPortTask->deleteLater();
        emit appLogMessage_signal("设备连接失败");
    }
}

void settingWidget::searchSerialPort()
{
    QStringList SerialPortNameList;/*保存搜索到的串口，存入列表中*/
    serialPortComboBox->clear();
    foreach (const QSerialPortInfo &SerialPortInfo, QSerialPortInfo::availablePorts()) /*遍历可使用的串口*/
    {
        SerialPortNameList.append(SerialPortInfo.portName());/*把搜索到的串口存入列表中*/
        qDebug() << SerialPortInfo.portName() + " " + SerialPortInfo.description();
    }
    serialPortComboBox->addItems(SerialPortNameList);/*将搜索到的串口显示到UI界面上*/
}

void settingWidget::closeSerialPort()
{
    serialPortTask->SerialPortClose();
    serialPortComboBox->clear();
    serialPortComboBox->setDisabled(false);

    serialPortThread->exit(0);
    serialPortThread->wait();
    serialPortThread->deleteLater();

    serialPortTask->deleteLater();

    openSerialPortPushButton->setIcon(waitOpenSerialPort);      /* 加载等待连接的图标 */
    openSerialPortPushButton->setText("打开串口");

    serialPortConnectState = false;

    emit appLogMessage_signal("串口已关闭");
}
