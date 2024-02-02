#ifndef WIDGET_H
#define WIDGET_H

#include <QPushButton>
#include <QWidget>
#include <QTextEdit>

#include "settingwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void settingButton_clicked_slot();                          /* 设置按键的单击的响应槽函数 */

    void showLogMessage_slot(QByteArray logMessage);            /* 显示输出log信息的槽函数 */
signals:
    void appLogMessage_signal(QByteArray logMessage);
private:
    Ui::Widget *ui;

    int mainWidgetWidth = 0;    /* 记录主窗口的大小 */
    int mainWidgetheight = 0;

    QPushButton *settingButton = nullptr;     /* 设置按钮 */
    QWidget *settingWindow = nullptr;         /* 设置窗口 */
    settingWidget *mSettingWindow = nullptr;  /* 实现设置窗口的各项功能的具体实现 */

    QTextEdit* logTextEdit;                   /* 输出应用内的log信息 */

    void windowInit();  /* 执行所有的窗口初始化操作 */
protected:
    virtual void resizeEvent(QResizeEvent *event) override;

    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

};
#endif // WIDGET_H
