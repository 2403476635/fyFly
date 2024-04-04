#ifndef FLYBOARDDATASHOWWIDGET_H
#define FLYBOARDDATASHOWWIDGET_H

#include <QTreeWidget>
#include <QWidget>
#include <QStandardItemModel>
#include <QCheckBox>
#include <QComboBox>
#include <QHeaderView>
#include <QStyleFactory>
#include <QLineEdit>
#include <QTableView>

class flyBoardDataShowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit flyBoardDataShowWidget(QWidget *parent = nullptr);

    void setWindowSize(int width, int height);      /* 设置窗口大小 */
signals:

private:
    QWidget *flyBoardDataShowWindow = nullptr;      /* 飞控数据显示界面 */

     QTreeWidget *flyBoardDataTreeWidget = nullptr; /* 数据显示的控件 */
};

#endif // FLYBOARDDATASHOWWIDGET_H
