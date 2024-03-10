#include "widget.h"

#include "ui_widget.h"
#include <QFileDialog>
#include <QIcon>
#include <QPushButton>
#include <QResizeEvent>
#include <QTabWidget>
#include <QDirIterator>

#include "loadModel/ModelWindow.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    windowInit();/* 窗口初始化 */
    excelWriteTest = new myexcel(this);
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

void Widget::flyModelShowShowButton_clicked_slot()
{
    if(true == modelWidget->isHidden())
    {
        modelWidget->show();
    }
    else
    {
        modelWidget->hide();
    }
}

void Widget::readImuDataButton_clicked_slot()
{
    readImuDataFlag = 1;
}

void Widget::showLogMessage_slot(QByteArray logMessage)
{
    logTextEdit->append(logMessage);
}

void Widget::imuDataDeal_slot(_imuDataStruct imuData)
{
    static int32_t readDataCount = 0;
    if(1 == readImuDataFlag && readDataCount < 4000)
    {
        dataGyrox.append(imuData.roll);
        dataGyroy.append(imuData.pitch);
        readDataCount++;
        qDebug() << readDataCount;
    }
    else if(readDataCount >= 4000)
    {
        excelWriteTest->saveData(dataGyrox,dataGyroy,dataGyroz);
        readImuDataFlag = 0;
        readDataCount = 0;
        qDebug() << "save done";
    }

    QByteArray dataShowString = "<html><font color='blue'>ICM TEMP" + QByteArray::number(imuData.icm42670pTemperature) + "</font><br>" +
                                "<font color='blue'>ICM ACC_X" + QByteArray::number(imuData.accel_x) + "</font><br>" +
                                "<font color='blue'>ICM ACC_Y" + QByteArray::number(imuData.accel_y) + "</font><br>" +
                                "<font color='blue'>ICM ACC_Z" + QByteArray::number(imuData.accel_z) + "</font><br>" +

                                "<font color='blue'>ICM GRY_X" + QByteArray::number(imuData.gyro_x) + "</font><br>" +
                                "<font color='blue'>ICM GRY_Y" + QByteArray::number(imuData.gyro_y) + "</font><br>" +
                                "<font color='blue'>ICM GRY_Z" + QByteArray::number(imuData.gyro_z) + "</font><br>" +

                                "<font color='blue'>ICM MAG_X" + QByteArray::number(imuData.mag_x) + "</font><br>" +
                                "<font color='blue'>ICM MAG_Y" + QByteArray::number(imuData.mag_y) + "</font><br>" +
                                "<font color='blue'>ICM MAG_Z" + QByteArray::number(imuData.mag_z) + "</font><br>" +

                                "<font color='blue'>PITCH" + QByteArray::number(imuData.pitch) + "</font><br>" +
                                "<font color='blue'>ROLL" + QByteArray::number(imuData.roll) + "</font><br>" +
                                "<font color='blue'>YAW" + QByteArray::number(imuData.yaw) + "</font><br>" +

                                "<font color='red'>SPL06 TEMP" + QByteArray::number(imuData.spl06Temperature) + "</font><br>" +
                                "<font color='red'>SPL06 PRESSURE" + QByteArray::number(imuData.barPressure) + "</font></html>";
    imuDataShowLabel->setText(dataShowString);
}

void Widget::flySystemInfoData_slot(_systemInfoStruct systemInfo)
{
    QByteArray dataShowString = "<html>";
    switch (systemInfo.flyState->flyLockState)
    {
        case FLY_LOCK:
        {
            dataShowString += "<font color='red'>LockState FLY_LOCK </font><br>";
            break;
        }
        case FLY_UNLOCK:
        {
            dataShowString += "<font color='red'>LockState FLY_UNLOCK </font><br>";
            break;
        }
        default:
            dataShowString += "<font color='red'>LockState unknow </font><br>";
            break;
    }

    switch (systemInfo.flyState->flyTakeOffState)
    {
        case MOTO_STOP:
        {
            dataShowString += "<font color='red'>flyTakeOffState MOTO_STOP </font><br>";
            break;
        }
        case MOTO_IDING:
        {
            dataShowString += "<font color='red'>flyTakeOffState MOTO_IDING </font><br>";
            break;
        }
        case LANDING_ING:
        {
            dataShowString += "<font color='red'>flyTakeOffState LANDING_ING </font><br>";
            break;
        }
        case LANDING_DONE:
        {
            dataShowString += "<font color='red'>flyTakeOffState LANDING_DONE </font><br>";
            break;
        }
        case WAIT_TAKE_OFF:
        {
            dataShowString += "<font color='red'>flyTakeOffState WAIT_TAKE_OFF </font><br>";
            break;
        }
        case TAKE_OFF_ING:
        {
            dataShowString += "<font color='red'>flyTakeOffState TAKE_OFF_ING </font><br>";
            break;
        }
        case TAKE_OFF_DONE:
        {
            dataShowString += "<font color='red'>flyTakeOffState TAKE_OFF_DONE </font><br>";
            break;
        }
        case EMERGENCY_STOP:
        {
            dataShowString += "<font color='red'>flyTakeOffState EMERGENCY_STOP </font><br>";
            break;
        }
        case ELECTRICAL_CAIBRATION:
        {
            dataShowString += "<font color='red'>LockState ELECTRICAL_CAIBRATION </font><br>";
            break;
        }
        default:
            dataShowString += "<font color='red'>flyTakeOffState unknow </font><br>";
            break;
    }
    dataShowString += "</html>";
    flySystemInfoDataShowLabel->setText(dataShowString);
}

void Widget::windowInit()
{
    this->setWindowTitle("fyFlyAssistant");     /* 设置窗口名称 */
    this->resize(800,600);
    this->setMinimumSize(800, 600);             /* 设置窗口最小尺寸 */

    /* 加载模型 */
    /* 这里这样实现是想试试应用内加载文件的方式 */
    if (tempDir.isValid())
    {
        const QString tempDirPath = tempDir.path();
        QDirIterator it("://3dFlyModel", QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            it.next();
            const QString relativeFilePath = it.filePath().mid(14); // 移除开头的 "://3dFlyModel/"
            const QString tempFilePath = tempDirPath + "/" + relativeFilePath;
            if (it.fileInfo().isDir()) {
                QDir().mkpath(tempFilePath);
            } else {
                QFile::copy(it.filePath(), tempFilePath);
            }
        }

        modelWidget = new ModelWindow(tempDirPath + "/model/Su-27.obj", this);
        modelWidget->resize(600, 400);
        modelWidget->setRotation(0, 0, 0);
        modelWidget->show();
    }


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

    /* 飞机模型显示按钮 */
    flyModelShowShowButton = new QPushButton(this);
    QIcon flyModelShowShowIcon("://images/button/flyModelButton.png");
    flyModelShowShowButton->setIconSize(QSize(32,32));
    flyModelShowShowButton->move(800-96,600-32);
    flyModelShowShowButton->setIcon(flyModelShowShowIcon);
    flyModelShowShowButton->setStyleSheet( "QPushButton{border:0px;background:white;}"
                                  "QPushButton:hover{border:0px;background:blue;}"
                                  "QPushButton:pressed{border:0px;background:red;}");

    readImuDataButton = new QPushButton(this);
    readImuDataButton->resize(32,32);
    readImuDataButton->setText("R");
    readImuDataButton->move(800-32*4,600-32);
    connect(readImuDataButton, &QPushButton::clicked,this,&Widget::readImuDataButton_clicked_slot);

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
    logTextEdit->setGeometry(0, 600, 400, 100); /* 设置QTextEdit的位置和大小 */
    logTextEdit->show();                        /* 将QTextEdit添加到主窗口 */

//    QSlider *xSlider = new QSlider(Qt::Horizontal,this);
//    xSlider->show();
//    xSlider->setRange(-180,180);
//    xSlider->setGeometry(100, 100, 200, 30);
//    xSlider->setSliderPosition(0);
//    connect(xSlider, &QSlider::valueChanged, [=](int value){
//        modelWidget->setRotation(value,180,0);
//        qDebug() << "Slider value: " << value;
//    });

//    QSlider *ySlider = new QSlider(Qt::Horizontal,this);
//    ySlider->show();
//    ySlider->setRange(-180,180);
//    ySlider->setGeometry(100, 200, 200, 30);

//    connect(ySlider, &QSlider::valueChanged, [=](int value){
//        modelWidget->setRotation(0,180 + value,0);
//        qDebug() << "Slider value: " << value;
//    });
//    ySlider->setSliderPosition(0);
//    QSlider *zSlider = new QSlider(Qt::Horizontal,this);
//    zSlider->show();
//    zSlider->setRange(-180,180);
//    zSlider->setGeometry(100, 300, 200, 30);
//    zSlider->setSliderPosition(0);
//    connect(zSlider, &QSlider::valueChanged, [=](int value){
//        modelWidget->setRotation(0,180,value);
//        qDebug() << "Slider value: " << value;
//    });

    imuDataShowLabel = new QLabel(this);
    imuDataShowLabel->setGeometry(600, 0, 200, 300);

    flySystemInfoDataShowLabel = new QLabel(this);
    flySystemInfoDataShowLabel->setGeometry(600, 200, 200, 200);

    connect(mSettingWindow,&settingWidget::appLogMessage_signal,this,&Widget::showLogMessage_slot);             /* 连接log信息打印的信号,信号发出者:mSettingWindow */
    connect(mSettingWindow,&settingWidget::imuData_signal,this,&Widget::imuDataDeal_slot);                      /* mSettingWindow中创建的串口线程发送的传感器数据到主界面串口进行显示 */
    connect(mSettingWindow,&settingWidget::flySystemInfoData_signal,this,&Widget::flySystemInfoData_slot);      /* mSettingWindow中创建的串口线程发送的飞控数据到主界面串口进行显示 */

    connect(settingButton, &QPushButton::clicked,this,&Widget::settingButton_clicked_slot);                     /* 连接设置按键的单击信号和槽 */
    connect(flyBoardDataShowButton, &QPushButton::clicked,this,&Widget::flyBoardDataShowButton_clicked_slot);   /* 连接飞控数据显示按键的单击信号和槽 */
    connect(flyModelShowShowButton, &QPushButton::clicked,this,&Widget::flyModelShowShowButton_clicked_slot);   /* 连接飞控模型显示按键的单击信号和槽 */
}

void Widget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    settingButton->move(this->width()-32,this->height()-32);
    flyBoardDataShowButton->move(this->width()-64,this->height()-32);
    flyModelShowShowButton->move(this->width()-96,this->height()-32);

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
