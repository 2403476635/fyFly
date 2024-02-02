#include "serialport.h"
#include "qapplication.h"
#include <QDebug>
#include <QFile>


SerialPortThread::SerialPortThread(QObject *parent)
    :QObject(parent)
{

}

SerialPortThread::SerialPortThread(QString PortName, int BaudRate, QSerialPort::DataBits PortDataBits, QSerialPort::Parity PortParityBits, QSerialPort::StopBits PortStopBits, QSerialPort::FlowControl PortFlowControl)
{
    mPortName = PortName;
    mBaudRate = BaudRate;
    mPortDataBits = PortDataBits;
    mPortParityBits = PortParityBits;
    mPortStopBits = PortStopBits;
    mPortFlowControl = PortFlowControl;
}

SerialPortThread::~SerialPortThread()
{

}

void SerialPortThread::openSerialPort()
{
    qDebug()<<"SerialPortThread threadId: "<<QThread::currentThreadId();
    SerialPort = new QSerialPort();                                         /*创建串口对象*/

    SerialPort->setPortName(mPortName);                                      /*设置选中的COM口*/
    SerialPort->setBaudRate(mBaudRate);                                      /*设置串口的波特率*/
    SerialPort->setDataBits(mPortDataBits);                                  /*设置数据位数*/
    SerialPort->setParity(mPortParityBits);                                  /*设置奇偶校验,NoParit无校验*/
    SerialPort->setStopBits(mPortStopBits);                                  /*设置停止位，OneStop一个停止位*/
    SerialPort->setFlowControl(mPortFlowControl);                            /*设置流控制，NoFlowControl无流控制*/

    if(SerialPort->open(QIODevice::ReadWrite) == true)                       /*ReadWrite设置的是可读可写的属性*/
    {

        SerialPortIsOK = true;
        emit OpenSerialPortOK_signal(true);                                  /*发送串口打开成功信号*/
    }
    else
    {
        SerialPortIsOK = false;
        emit OpenSerialPortOK_signal(false);                                 /*发送串口打开失败信号*/
    }

    connect(SerialPort,&QSerialPort::readyRead,this,&SerialPortThread::DataRead);               /*关联串口数据接收*/

    bufferObj = new ringbufer;
    serialPortBuffer = new RingBuff_t;
    bufferObj->RingBuffer_Init(serialPortBuffer);

}

void SerialPortThread::SerialPortClose()
{
    if(SerialPort->isOpen() == true)
    {
        SerialPort->close();        /*关闭串口*/
        SerialPort->deleteLater();
    }
    qDebug("关闭串口设备");
}
/* 读取串口数据 */
void SerialPortThread::DataRead()
{
    QByteArray receiveData = SerialPort->readAll();
    unsigned int writeDataLen = 0;
    bufferObj->WriteBytes(serialPortBuffer,receiveData,&writeDataLen);
    if(writeDataLen != receiveData.size())
    {
        qDebug("1 写入数据不完整");
    }
    QByteArray bufferData;
    unsigned int nowBufferSize = bufferObj->ValidDataLen(serialPortBuffer);
    bufferObj->ReadBytes(serialPortBuffer,&bufferData,nowBufferSize);/*读取缓冲区中的所有数据*/
    unsigned int startFramePlace = 0;
    unsigned int endFramePlace = 0;

    for(unsigned int i=0;i<bufferData.size();)
    {
        if((uint8_t)bufferData.at(i) == 0xAA)
        {
            startFramePlace = i;
            if((nowBufferSize - i) > 3)
            {
//                qDebug() << "getFrameHead";
//                qDebug() << "nowBufferSize = " << nowBufferSize;
//                qDebug() << "i + 3 + bufferData.at(i+3) + 2 = " << i + 3 + bufferData.at(i+3) + 2;
                if(nowBufferSize >= (unsigned int)( i + 3 + (uint8_t)bufferData.at(i+3) + 2))
                {
                    endFramePlace = i + 4 + (uint8_t)bufferData.at(i+3) + 2;
                    QByteArray frameData = bufferData.mid(startFramePlace,endFramePlace - startFramePlace);
                    if(FrameDataCheck(frameData) == true)/*检验帧数据是否正确*/
                    {
                        bufferData.remove(i,frameData.size());
                        /***********************得到下位机的完整数据**********************************/
                        if((unsigned char)frameData.at(1) == MCU)
                        {
                            switch((unsigned char)frameData.at(2))
                            {
                                case CMD_LOG_MESSAGE:   /* 设备发送的LOG信息 */
                                {
//                                    qDebug() << frameData.mid(4,frameData.at(3));
//                                    qDebug() << frameData.toHex(' ').toUpper();
                                    emit appLogMessage_signal(frameData.mid(4,(uint8_t)frameData.at(3)));
                                    break;
                                }
                                case CMD_JPEG_DATA_PACK_SIZE:   /* 设备发送的图片大小信息 */
                                {
                                    jpegData.clear();
                                    getJpegDataSize = (uint8_t)frameData.at(4) | (uint8_t)frameData.at(5) << 8 |
                                                       (uint8_t)frameData.at(6)  << 16 | (uint8_t)frameData.at(7)  << 24;
                                    QByteArray logMessage = "图片文件大小 = " + QByteArray::number(getJpegDataSize);
                                    emit appLogMessage_signal(logMessage);
                                    break;
                                }
                                case CMD_JPEG_DATA_PACK:
                                {
                                    jpegData += frameData.mid(4,(uint8_t)frameData.at(3));
                                    float percent = (float)jpegData.size()/(float)getJpegDataSize * 100.0;
                                    QByteArray tempData = "接收进度:" + QByteArray::number(percent) + "%";
                                    emit appLogMessage_signal(tempData);
                                    break;
                                }
                                case CMD_JPEG_DATA_SEND_END:
                                {
                                    QByteArray logMessage = "图片接收完成";
                                    emit appLogMessage_signal(logMessage);
                                    emit pictureData_signal(jpegData);

                                    QString filePath = QCoreApplication::applicationDirPath() + "/" + "test.jpg";
                                    QFile *RecFile = new QFile(filePath);
                                    RecFile->open(QFile::WriteOnly);
                                    RecFile->write(jpegData);
                                    RecFile->close();
                                    delete RecFile;
                                    RecFile = NULL;

                                    break;
                                }
                                case CMD_NOW_APP_STATE:
                                {
                                    emit deviceAppState_signal();
                                    break;
                                }
                                case CMD_NOW_IAP_STATE:
                                {
                                    emit deviceIapState_signal();
                                    break;
                                }
                                default:
                                    break;
                            }
                        }
                        i = 0;
                        continue;
                    }
                }
                else
                {
                    bufferObj->WriteBytes(serialPortBuffer,bufferData,&writeDataLen);
                    if(writeDataLen != bufferData.size())
                    {
                        qDebug("3 写入数据不完整");
                    }
                    break;
                }
            }
            else
            {
                bufferObj->WriteBytes(serialPortBuffer,bufferData,&writeDataLen);
                if(writeDataLen != bufferData.size())
                {
                    qDebug("4 写入数据不完整");
                }
                break;
            }
        }
        i++;
    }
}
int SerialPortThread::QByteToUint(QByteArray Data)
{
    int IntTemp = 0;
    IntTemp  = (Data[0] & 0x00FF);
    IntTemp |= ((Data[1]<<8 ) & 0xFF00);
    IntTemp |= ((Data[2]<<16 ) & 0xFF0000);
    IntTemp |= ((Data[3]<<24 ) & 0xFF000000);

    return IntTemp;
}
void SerialPortThread::DataSend(QByteArray SendDataArray)
{
    if(SerialPortIsOK == true)
    {
       SerialPort->write(SendDataArray);
    }
}

bool SerialPortThread::SerialPortState()
{
    return SerialPortIsOK;
}

void SerialPortThread::SerialPortSetState(bool State)
{
    SerialPortIsOK = State;
}

void SerialPortThread::sendBinFile(QByteArray binFileData)
{
    QByteArray tempData;
    uint32_t dataLen = binFileData.size();
    tempData.resize(4);
    tempData[0] = dataLen;
    tempData[1] = dataLen >>8;
    tempData[2] = dataLen >>16;
    tempData[3] = dataLen >>24;

    if(SerialPortIsOK == true)
    {
        SerialPort->write(setSerialPortStringDataFormat(0xAA,SerialPortThread::frameAddress::PC,SerialPortThread::frameCmd::CMD_SEND_BIN_FILE_SIZE,tempData));

        QByteArray data;
        data.resize(dataLen);

        for(uint32_t i=0;i<dataLen;i+=4)
        {
            data[i+0] = binFileData.at(i + 0);
            data[i+1] = binFileData.at(i + 1);
            data[i+2] = binFileData.at(i + 2);
            data[i+3] = binFileData.at(i + 3);
        }
        uint32_t clcTimes = 0;
        clcTimes = dataLen / 240;
        for(uint32_t i=0;i<clcTimes;i++)
        {
            QByteArray tempData = data.mid(0,240);
            SerialPort->write(setSerialPortStringDataFormat(0xAA,SerialPortThread::frameAddress::PC,SerialPortThread::frameCmd::CMD_SEND_BIN_FILE_PACK,tempData));
            data.remove(0,240);
        }

        tempData.clear();
        tempData = data.mid(0,dataLen%240);

        SerialPort->write(setSerialPortStringDataFormat(0xAA,SerialPortThread::frameAddress::PC,SerialPortThread::frameCmd::CMD_SEND_BIN_FILE_PACK,tempData));
        data.remove(0,dataLen%240);
        SerialPort->write(setSerialPortStringDataFormat(0xAA,SerialPortThread::frameAddress::PC,SerialPortThread::frameCmd::CMD_SEND_BIN_FILE_END,0));
    }
}

bool SerialPortThread::FrameDataCheck(QByteArray Frame)
{
    unsigned char sumcheck = 0;
    unsigned char addcheck = 0;

    //qDebug() << Frame;
    for(uint8_t i=0; i < (uint8_t)Frame[3] + 4 ; i++)
    {
        sumcheck += (uint8_t)Frame[i]; //从帧头开始，对每一字节进行求和，直到DATA区结束
        addcheck += sumcheck; //每一字节的求和操作，进行一次sumcheck的累加
    }
    uint8_t Len = (uint8_t)Frame[3];
//    qDebug() << "计算的sumcheck = " << sumcheck;
//    qDebug() << "计算的addcheck = " << addcheck;
//    qDebug() << "帧中sumcheck = " << (uint8_t)Frame.at(Len + 4);
//    qDebug() << "帧中addcheck = " << (uint8_t)Frame.at(Len + 5);

    if(sumcheck == (uint8_t)Frame.at(Len + 4) && addcheck == (uint8_t)Frame.at(Len + 5))
    {
        return true; //校验通过
    }
    else
    {
        emit appLogMessage_signal("接收数据帧校验失败");
        return false; //校验失败
    }
}

/* 设置串口发送一个字符串的帧 */
QByteArray SerialPortThread::setSerialPortStringDataFormat(unsigned char frameHead, unsigned char frameAddress, unsigned char frameID, QByteArray data)
{
    unsigned char frameDataLen = 0;

    QByteArray frameData;
    frameDataLen = data.size();
    frameData.resize(frameDataLen + 4 + 2);
    frameData[0] = frameHead;                               /* 帧头 */
    frameData[1] = frameAddress;                            /* 帧地址 */
    frameData[2] = frameID;                                 /* 帧ID */
    frameData[3] = frameDataLen;                            /* 帧数据长度 */

    for(int i=0;i<frameDataLen;i++)
    {
        frameData[4+i] = data.at(i);
    }

    unsigned char sumcheck = 0;
    unsigned char addcheck = 0;
    for(unsigned char i=0; i < (uint8_t)frameData[3] + 4 ; i++)
    {
        sumcheck += (uint8_t)frameData[i]; //从帧头开始，对每一字节进行求和，直到DATA区结束
        addcheck += sumcheck; //每一字节的求和操作，进行一次sumcheck的累加
    }
    frameData[frameDataLen + 4] = sumcheck;
    frameData[frameDataLen + 5] = addcheck;
    return frameData;
}


