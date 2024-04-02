#ifndef WAVEWIDGET_H
#define WAVEWIDGET_H

#include <QWidget>
#include <QFile>
#include <QTextStream>

#include "common.h"
#include "qcustomplot.h"

class waveWidget : public QWidget
{
    Q_OBJECT
public:
    explicit waveWidget(QWidget *parent = nullptr);

    void windowInit();

    void imuDataDeal_slot(_imuDataStruct imuData);
signals:

private slots:
    void xAxisScrollBar_slot(int xAxisScrollBarValue);  /* 滑块的数值变化槽函数 */

    void readImuDataButton_clicked_slot();              /* 读取/保存发送的波形数据的按键的槽函数 */
    void pauseWaveButtonButton_clicked_slot();          /* 暂停/继续波形的按键的槽函数 */
private:
    QCustomPlot *waveShowPlot = nullptr;
    QWidget *waveShowWindow = nullptr;

    QPushButton *readImuDataButton = nullptr;   /* 记录数据按键 */
    QPushButton *pauseWaveButton = nullptr;         /* 暂停/开始显示波形按键 */

    QScrollBar *xAxisScrollBar = nullptr;

    QList<QCheckBox*> dataShowCheckBox;

    uint8_t readImuDataFlag = 0;
    QList<float> dataGyrox;
    QList<float> dataGyroy;
    QList<float> dataGyroz;

    QTimer *refreshTimer;

    uint8_t pauseWaveDataFlag = 0;
protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // WAVEWIDGET_H
