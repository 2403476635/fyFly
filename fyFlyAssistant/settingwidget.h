#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QLineEdit>

#include "mycombobox.h"
#include "serialport.h"
#include "common.h"

class settingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit settingWidget(QWidget *parent = nullptr);

    ~settingWidget();

    bool serialPortConnectState = false;            /* 保存设备的连接状态 */

    QString openedSerialPortName;                   /* 保存连接的串口号 */

    void setWindowSize(int width, int height);      /* 设置窗口大小 */

    void windowShow();                              /* 窗口显示 */

    void windowHide();                              /* 窗口隐藏 */

    bool isHidden() const;                          /* 当前的窗口状态 */

    void searchSerialPort();                        /* 执行一次串口扫描 */

    void closeSerialPort();                         /* 关闭串口 */

private slots:
    void openSerialPortPushButton_clicked_slot();   /* 打开串口响应槽函数 */

    void openSerialPortState_slot(bool state);      /* 对串口线程反馈的串口状态进行处理 */
signals:
    void closeSerialPort_signal();

    void sendSerialPortData(QByteArray serialPortData);

    void appLogMessage_signal(QByteArray logMessage);

    void imuData_signal(_imuDataStruct imuData);     /* 传感器的数据 */
    void flySystemInfoData_signal(_systemInfoStruct systemInfo);  /* 飞机的系统信息 */
private:
    QIcon openSerialPortSuccess;         /* 串口连接成功显示的图标 */
    QIcon waitOpenSerialPort;            /* 等待串口连接时显示的图标 */

    QWidget *settingWindow = nullptr;

    QTabWidget *settingTabWidget = nullptr;

    QGroupBox *serialPortgroupBox = nullptr;            /* 串口连接设置组 */
    QLineEdit *serialPortRateLineEdit = nullptr;        /* 设置串口的通信波特率 */
    QPushButton *openSerialPortPushButton = nullptr;    /* 打开串口的按键 */
    myComboBox *serialPortComboBox = nullptr;           /* 显示搜索到的串口端口 */

    QThread *serialPortThread = nullptr;
    SerialPortThread *serialPortTask = nullptr;         /* 处理串口的线程 */


};

#endif // SETTINGWIDGET_H
