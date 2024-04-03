#ifndef WIDGET_H
#define WIDGET_H

#include <QPushButton>
#include <QWidget>
#include <QTextEdit>
#include <QTemporaryDir>
#include <QLabel>

#include "wavewidget.h"
#include "settingwidget.h"
#include "flyboarddatashowwidget.h"
#include "parametersettingwidget.h"
#include "common.h"
#include "ellipsoidfitwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class ModelWindow;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void settingButton_clicked_slot();                          /* 设置按键的单击的响应槽函数 */
    void flyBoardDataShowButton_clicked_slot();                 /* 飞控数据显示按键的单击响应槽函数 */
    void flyModelShowShowButton_clicked_slot();                 /* 飞控模型显示按键的单击响应槽函数 */
    void dataWaveShowShowButton_clicked_slot();                 /* 数据波形显示按键的单击响应槽函数 */
    void ellipsoidfitShowShowButton_clicked_slot();             /* 矫正窗口显示按键的单击响应槽函数 */

    void showLogMessage_slot(QByteArray logMessage);            /* 显示输出log信息的槽函数 */

    void imuDataDeal_slot(_imuDataStruct imuData);              /* 处理接收的传感器数据 */
    void flySystemInfoData_slot(_systemInfoStruct systemInfo);  /* 处理接收的飞机系统信息 */

signals:
    void appLogMessage_signal(QByteArray logMessage);
private:
    Ui::Widget *ui;

    int mainWidgetWidth = 0;    /* 记录主窗口的大小 */
    int mainWidgetheight = 0;

    QPushButton *settingButton = nullptr;               /* 设置按钮 */
    QPushButton *flyBoardDataShowButton = nullptr;      /* 飞控数据显示按钮 */
    QPushButton *flyModelShowButton = nullptr;      /* 飞控姿态模型显示按钮 */
    QPushButton *dataWaveShowButton = nullptr;          /* 数据波形显示按钮 */
    QPushButton *ellipsoidfitShowButton = nullptr;      /* 矫正窗口按键 */
    QPushButton *setFlyParameterShowButton = nullptr;   /* 参数设置窗口按键 */

    QLabel *imuDataShowLabel = nullptr;                 /* 显示传感器的数据 */
    QLabel *flySystemInfoDataShowLabel = nullptr;       /* 显示飞控系统的数据 */

    QWidget *settingWindow = nullptr;                   /* 设置窗口 */
    QWidget *flyDataShowWindow = nullptr;               /* 飞控数据显示窗口 */

    settingWidget *mSettingWindow = nullptr;                    /* 设置窗口的各项功能的具体实现 */
    flyBoardDataShowWidget *mflyBoardDataShowWidget = nullptr;  /* 飞控数据显示窗口的各项功能的具体实现 */
    waveWidget *mWaveWindow = nullptr;                          /* 波形显示窗口的各项功能的具体实现 */
    paramterSettingWidget *mParamterSettingWidget = nullptr;    /* 参数设置窗口的具体实现 */
    ellipsoidfitWidget *mEllipsoidfitWidget = nullptr;          /* 椭球拟合窗口的具体实现 */

    QTextEdit* logTextEdit;                         /* 输出应用内的log信息 */

    QTemporaryDir tempDir;
    ModelWindow *modelWidget = nullptr;             /* opengl的实现窗口 */

    void windowInit();  /* 执行所有的窗口初始化操作 */
protected:
    virtual void resizeEvent(QResizeEvent *event) override;

    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

    void closeEvent(QCloseEvent *event) override;
};
#endif // WIDGET_H
