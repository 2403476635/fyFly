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
private slots:
    void openFileButton_clicked_slot();
    void readCorrectParameterButton_clicked_slot();
    void sendCorrectParameterButton_clicked_slot();
    void clearDataShowButton_clicked_slot();
signals:
    void correctSend_signal(QByteArray data);

private:
    QPushButton *openFileButton = nullptr;              /* 打开原始数据参数按键 */
    QPushButton *readCorrectParameterButton = nullptr;  /* 读取校准参数按键 */
    QPushButton *sendCorrectParameterButton = nullptr;           /* 发送校准参数按键 */
    QPushButton *clearDataShowButton = nullptr;           /* 清除界面上显示的数据按键 */

    QProgressBar *mProgressBar = nullptr;

    QList<double> ellipsoidData;
    QList<QLabel*> showDataLabel;

    bool isEllipsoidFitDone = false;

    void ellipsoidFit(const char * filename_in, const char * filename_out, int mag_data_counter);

    QByteArray setSerialPortStringDataFormat(uint8_t frameHead, uint8_t frameAddress, uint8_t frameID, QByteArray data);

};

#endif // ELLIPSOIDFITWIDGET_H
