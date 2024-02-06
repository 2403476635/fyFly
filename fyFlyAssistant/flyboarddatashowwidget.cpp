#include "flyboarddatashowwidget.h"

flyBoardDataShowWidget::flyBoardDataShowWidget(QWidget *parent)
    : QWidget{parent}
{
    flyBoardDataShowWindow = parent;
    flyBoardDataShowWindow->setStyleSheet("background-color: red;");
    flyBoardDataShowWindow->setWindowFlags(Qt::FramelessWindowHint); /* 设置窗口为无边框 */
    flyBoardDataShowWindow->hide();

    flyBoardDataTreeWidget = new QTreeWidget(flyBoardDataShowWindow);
    flyBoardDataTreeWidget->resize(QSize(800,300));
    flyBoardDataTreeWidget->setStyle(QStyleFactory::create("windows"));                           /*设置让节点之间显示连线*/
    flyBoardDataTreeWidget->setStyleSheet(QString(                                  /*设置TreeWidget的样式*/
                                      " QTreeWidget::item {"
                                      "     border-left:   1px solid rgb(200,200,200);"
                                      "     border-bottom: 1px solid rgb(200,200,200);"
                                      " }"
                                      "QTreeWidget{"
                                      "    color: black;"
                                      "    background:rgb(255,255,255);"
                                      "}"
                                      "QTreeWidget::item:selected {"
                                      "    color: black;"
                                      "    background:rgb(255,255,255);"
                                      //"border: 1px solid rgb(65,173,255);"
                                      "}"
                                      ));
    /*表头设置*/
    flyBoardDataTreeWidget->setHeaderLabels(QStringList()<<"帧ID"<<"数据位"<<"数据类型"<<"数据缩放");
    flyBoardDataTreeWidget->header()->setStretchLastSection(false);                  /*取消最后一列自适应列宽*/
    flyBoardDataTreeWidget->header()->setDefaultAlignment(Qt::AlignCenter);          /*设置居中显示*/
    flyBoardDataTreeWidget->header()->resizeSection(0,80);
    flyBoardDataTreeWidget->header()->resizeSection(1,80);
    flyBoardDataTreeWidget->header()->resizeSection(2,100);
    flyBoardDataTreeWidget->header()->resizeSection(3,100);                          /*设置列宽*/
    flyBoardDataTreeWidget->header()->setSectionResizeMode(0,QHeaderView::Fixed);
    flyBoardDataTreeWidget->header()->setSectionResizeMode(1,QHeaderView::Fixed);
    flyBoardDataTreeWidget->header()->setSectionResizeMode(2,QHeaderView::Fixed);
    flyBoardDataTreeWidget->header()->setSectionResizeMode(3,QHeaderView::Fixed);    /*表头指定列定宽*/
    flyBoardDataTreeWidget->header()->setSectionsMovable(false);                     /*设置表头内容不能拖动*/



    //创建父节点
    QList<QTreeWidgetItem*> TopItems;   /*父节点*/
    QList<QCheckBox*> TopItemCheckBoxs; /*父节点的CheckBox*/
    QList<QTreeWidgetItem*> ChildItems;/*子节点*/
    QList<QComboBox*> DataType_ChildItemComboBoxs;      /*数据类型ComboBox*/
    QList<QComboBox*> Datazoom_ChildItemComboBoxs;      /*数据缩放ComboBox*/

    for (int i = 0; i < 10; ++i)
    {
        QString TopItemName = "F" + QString::number(i);
        TopItems.append(new QTreeWidgetItem(QStringList() << TopItemName << "使能该帧"));
//        TopItems.at(i)->setTextAlignment(0,Qt::AlignHCenter);
//        TopItems.at(i)->setTextAlignment(1,Qt::AlignHCenter);                   /*设置居中显示*/

        flyBoardDataTreeWidget->addTopLevelItem(TopItems.at(i));         /*添加父节点*/
//        TopItemCheckBoxs.append(new QCheckBox());

//        flyBoardDataTreeWidget->setItemWidget(TopItems.at(i),1,TopItemCheckBoxs.at(i));

//        TopItems.at(i)->setFirstColumnSpanned(true);
    }

    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            QString ChildFrameItemName = "F" + QString::number(i);
            QString ChildIndexItemName = QString::number(j);
            ChildItems.append(new QTreeWidgetItem(QStringList() << ChildFrameItemName << ChildIndexItemName));  /*生成子节点*/
            ChildItems.at(10*i +j)->setTextAlignment(0,Qt::AlignCenter);
            ChildItems.at(10*i +j)->setTextAlignment(1,Qt::AlignCenter);                                        /*设置居中显示*/

            DataType_ChildItemComboBoxs.append(new QComboBox());
            Datazoom_ChildItemComboBoxs.append(new QComboBox());                                                /*为子节点添加ComboBox*/
            DataType_ChildItemComboBoxs.at(10*i+j)->addItems(QStringList()  << "Int8"  << "Uint8"               /*数据类型ComboBox添加元素*/
                                                                            << "Int16" << "Uint16"
                                                                            << "Int32" << "Uint32");
            Datazoom_ChildItemComboBoxs.at(10*i+j)->addItems(QStringList()  << "1.0E+0"  << "1.0E+1"
                                                                            << "1.0E+2"  << "1.0E+3"
                                                                            << "1.0E+4"  << "1.0E+5");
            TopItems.at(i)->addChild(ChildItems.at(10 *i + j));                                                 /*添加子节点*/
            //TopItems.at(i)->setSizeHint();
            flyBoardDataTreeWidget->setItemWidget((ChildItems.at(i*10+j)),2,DataType_ChildItemComboBoxs.at(10*i + j));/*添加数据类型ComboBox*/
            flyBoardDataTreeWidget->setItemWidget((ChildItems.at(i*10+j)),3,Datazoom_ChildItemComboBoxs.at(10*i + j));/*添加数据缩放ComboBox*/
        }
    }
    flyBoardDataTreeWidget->setColumnWidth(0,80);
    flyBoardDataTreeWidget->setColumnWidth(1,80);
    flyBoardDataTreeWidget->setColumnWidth(2,100);
    flyBoardDataTreeWidget->setColumnWidth(3,100);

}

void flyBoardDataShowWidget::setWindowSize(int width, int height)
{
    flyBoardDataShowWindow->resize(width, height);

}

void flyBoardDataShowWidget::windowShow()
{
    flyBoardDataShowWindow->show();
}

void flyBoardDataShowWidget::windowHide()
{
    flyBoardDataShowWindow->hide();
}

bool flyBoardDataShowWidget::isHidden() const
{
    return flyBoardDataShowWindow->isHidden();
}
