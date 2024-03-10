#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QWidget>
#include <windows.h>
#include <QAbstractNativeEventFilter>
#include <dbt.h>

#include "common.h"
#include "ringbufer.h"

class SerialPortThread : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortThread(QObject *parent = nullptr);

    SerialPortThread(   QString PortName,
                        int BaudRate,
                        QSerialPort::DataBits PortDataBits,
                        QSerialPort::Parity PortParityBits,
                        QSerialPort::StopBits PortStopBits,
                        QSerialPort::FlowControl PortFlowControl);
    ~SerialPortThread();

    void openSerialPort();

    void SerialPortClose();                             /*串口关闭*/

    void DataRead();                                    /*串口接收数据处理*/

    void DataSend(QByteArray SendDataArray);            /*串口发送数据处理*/

    bool SerialPortState();

    void SerialPortSetState(bool State);

    void sendBinFile(QByteArray binFileData);

signals:

    void OpenSerialPortOK_signal(bool IsOpen);

    void ReadData_signal(QByteArray ReadData);

    void getData_signal(signed int temperature,signed int pressure,signed int filterValue);

    void appLogMessage_signal(QByteArray logMessage);

    void deviceIapState_signal();

    void deviceAppState_signal();

    void imuData_signal(_imuDataStruct imuData);        /* 传感器的数据 */
    void flySystemInfoData_signal(_systemInfoStruct systemInfo);  /* 飞机的系统信息 */

private:
    QString mPortName;
    int mBaudRate;
    QSerialPort::DataBits mPortDataBits;
    QSerialPort::Parity mPortParityBits;
    QSerialPort::StopBits mPortStopBits;
    QSerialPort::FlowControl mPortFlowControl;

    QSerialPort *SerialPort;

    bool SerialPortIsOK = false;

    bool FrameDataCheck(QByteArray Frame);

    int QByteToUint(QByteArray Data);

    RingBuff_t *serialPortBuffer;

    ringbufer *bufferObj;

    uint32_t getJpegDataSize = 0;   /* 收到设备发送来的JPEG图片大小 */

    QByteArray jpegData;            /* 存放当前接受的JPEG图片数据 */

    QByteArray setSerialPortStringDataFormat(unsigned char frameHead, unsigned char frameAddress, unsigned char frameID, QByteArray data);

    _imuDataStruct imuData;
    _flyStateStruct flyState;
    _systemInfoStruct systemInfo;                         /* 记录飞机的系统信息 */

};



#endif // SERIALPORT_H
