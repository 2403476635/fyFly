#ifndef PARAMETERSETTINGWIDGET_H
#define PARAMETERSETTINGWIDGET_H

#include "common.h"
#include <QWidget>

class paramterSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit paramterSettingWidget(QWidget *parent = nullptr);

    void windowInit();
public slots:
    void pidParameterShow_slot(QList<_pid> data);
signals:
    void sendParameterData_signal(QByteArray parameterData);

private slots:
    void readParameterButton_clicked_slot();    /* 读取参数按键的槽函数 */
    void setParameterButton_clicked_slot();     /* 设置参数按键的槽函数 */
    void clearDataShowButton_clicked_slot();    /* 清除界面上显示的数据按键的槽函数 */
private:
    QLabel *titleLabel1 = nullptr;
    QLabel *titleLabel2 = nullptr;
    QLabel *titleLabel3 = nullptr;
    /* 内环的标签和文本输入框 */
    QLabel *innerRingPitchLabel = nullptr;
    QLabel *innerRingRollLabel = nullptr;
    QLabel *innerRingYawLabel = nullptr;

    QLineEdit *innerRingPitchKpLineEdit = nullptr;
    QLineEdit *innerRingPitchKiLineEdit = nullptr;
    QLineEdit *innerRingPitchKdLineEdit = nullptr;

    QLineEdit *innerRingRollKpLineEdit = nullptr;
    QLineEdit *innerRingRollKiLineEdit = nullptr;
    QLineEdit *innerRingRollKdLineEdit = nullptr;

    QLineEdit *innerRingYawKpLineEdit = nullptr;
    QLineEdit *innerRingYawKiLineEdit = nullptr;
    QLineEdit *innerRingYawKdLineEdit = nullptr;
    /* 外环的标签和文本输入框 */
    QLabel *outerRingPitchLabel = nullptr;
    QLabel *outerRingRollLabel = nullptr;
    QLabel *outerRingYawLabel = nullptr;

    QLineEdit *outerRingPitchKpLineEdit = nullptr;
    QLineEdit *outerRingPitchKiLineEdit = nullptr;
    QLineEdit *outerRingPitchKdLineEdit = nullptr;

    QLineEdit *outerRingRollKpLineEdit = nullptr;
    QLineEdit *outerRingRollKiLineEdit = nullptr;
    QLineEdit *outerRingRollKdLineEdit = nullptr;

    QLineEdit *outerRingYawKpLineEdit = nullptr;
    QLineEdit *outerRingYawKiLineEdit = nullptr;
    QLineEdit *outerRingYawKdLineEdit = nullptr;

    QPushButton *readParameterButton = nullptr;     /* 读取参数按键 */
    QPushButton *setParameterButton = nullptr;      /* 设置参数按键 */
    QPushButton *clearDataShowButton = nullptr;           /* 清除界面上显示的数据按键 */
    /* 数据封帧 */
    QByteArray setSerialPortStringDataFormat(uint8_t frameHead, uint8_t frameAddress, uint8_t frameID, QByteArray data);
};

#endif // PARAMETERSETTINGWIDGET_H
