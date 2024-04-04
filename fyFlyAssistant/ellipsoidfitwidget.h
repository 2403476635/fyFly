#ifndef ELLIPSOIDFITWIDGET_H
#define ELLIPSOIDFITWIDGET_H

#include <QWidget>

#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include "common.h"

class ellipsoidfitWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ellipsoidfitWidget(QWidget *parent = nullptr);
    void windowInit();
public slots:
    void showFlyBoardCalibrationParameter_slot(QList<double> data);
    void originalSensorForCalibrationData_slot(_originalSensorForCalibrationData data);
private slots:
    void calculateParameterButton_clicked_slot();
    void readCorrectParameterButton_clicked_slot();
    void sendCorrectParameterButton_clicked_slot();
    void clearDataShowButton_clicked_slot();
    void setCalibrationButtonButton_clicked_slot();

signals:
    void correctSend_signal(QByteArray data);

private:
    QPushButton *calculateParameterButton = nullptr;                  /* 打开原始数据参数按键 */
    QPushButton *readCorrectParameterButton = nullptr;      /* 读取校准参数按键 */
    QPushButton *sendCorrectParameterButton = nullptr;      /* 发送校准参数按键 */
    QPushButton *clearDataShowButton = nullptr;             /* 清除界面上显示的数据按键 */
    QPushButton *setCalibrationButton = nullptr;            /* 设置校准模式按键 */

    QLabel *label1 = nullptr;
    QLabel *label2 = nullptr;
    QProgressBar *mProgressBar = nullptr;

    QComboBox *mChooseCalibrationModeComboBox = nullptr;    /* 校准模式选择下拉框 */

    QList<double> ellipsoidData;
    QList<QLabel*> showDataLabel;

    bool isEllipsoidFitDone = false;

    uint8_t nowCalibrationMode = 0;

    QLineEdit *mReadDataCountLineEdit = 0;

    uint32_t readDataCount = 0;                                     /* 保存需要读取的校准数据个数 */
    QList<_originalSensorForCalibrationData> originalSensorData;    /* 保存发送来的校准数据 */
    bool isRecordFlag = false;

    void ellipsoidFit(const char * filename_in, const char * filename_out, int mag_data_counter);
    void ellipsoidFit(QList<_originalSensorForCalibrationData> data, int mag_data_counter);

    QByteArray setSerialPortStringDataFormat(uint8_t frameHead, uint8_t frameAddress, uint8_t frameID, QByteArray data);

};

#endif // ELLIPSOIDFITWIDGET_H
