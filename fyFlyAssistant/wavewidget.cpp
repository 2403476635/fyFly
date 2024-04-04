#include "wavewidget.h"
#include <Eigen/Core>
#include <Eigen/Eigenvalues>

waveWidget::waveWidget(QWidget *parent)
    : QWidget{parent}
{

}

void waveWidget::windowInit()
{
    this->setWindowTitle("波形");
    this->setMinimumSize(800, 600);
    waveShowPlot = new QCustomPlot(this);
    waveShowPlot->setOpenGl(true);                  /* 使能OPENGL的使用 */
    qDebug()<<"opengl open"<<waveShowPlot->openGl();
    // 添加数据到曲线
    for(uint8_t i=0;i<9;i++)
    {
        waveShowPlot->addGraph();
    }
    /* 设置曲线颜色 */
    waveShowPlot->graph(0)->setPen(QPen(Qt::red));
    waveShowPlot->graph(1)->setPen(QPen(Qt::darkRed));
    waveShowPlot->graph(2)->setPen(QPen(Qt::green));
    waveShowPlot->graph(3)->setPen(QPen(Qt::darkGreen));
    waveShowPlot->graph(4)->setPen(QPen(Qt::blue));
    waveShowPlot->graph(5)->setPen(QPen(Qt::darkBlue));
    waveShowPlot->graph(6)->setPen(QPen(Qt::darkGreen));
    waveShowPlot->graph(7)->setPen(QPen(Qt::blue));
    waveShowPlot->graph(8)->setPen(QPen(Qt::darkBlue));
    // 设置图形样式
    waveShowPlot->xAxis->setLabel("X轴");
    waveShowPlot->yAxis->setLabel("Y轴");

    waveShowPlot->yAxis->rescale();
    waveShowPlot->xAxis->setRange(0, SHOW_RANGE);
    waveShowPlot->setGeometry(0,0,this->width()-80,this->height()-64);

    /* 用于改变坐标轴的滑块 */
    xAxisScrollBar = new QScrollBar(Qt::Horizontal,this);
    xAxisScrollBar->show();
    xAxisScrollBar->setRange(0,8000);
    xAxisScrollBar->setValue(0);
    xAxisScrollBar->setPageStep(8000);
    xAxisScrollBar->setGeometry(0, this->height()-64, this->width()-80, 32);
    connect(xAxisScrollBar, &QScrollBar::valueChanged, this, &waveWidget::xAxisScrollBar_slot);

    /* 定时刷新的定时器 */
    refreshTimer = new QTimer(this);
    refreshTimer->setInterval(1);       /* 1000Hz刷新 */
    connect(refreshTimer, &QTimer::timeout, this, [=]()
    {
        waveShowPlot->replot();
        // 定时器超时时执行的操作
    });
    refreshTimer->start();

    /* 记录数据按键 */
    readImuDataButton = new QPushButton(this);
    readImuDataButton->resize(100,32);
    readImuDataButton->setText("记录数据");
    readImuDataButton->move(0,this->height()-32);
    connect(readImuDataButton, &QPushButton::clicked,this,&waveWidget::readImuDataButton_clicked_slot);

    pauseWaveButton = new QPushButton(this);
    pauseWaveButton->resize(100,32);
    pauseWaveButton->setText("暂停波形");
    pauseWaveButton->move(readImuDataButton->pos().x()+readImuDataButton->width(),this->height()-32);
    connect(pauseWaveButton, &QPushButton::clicked,this,&waveWidget::pauseWaveButtonButton_clicked_slot);

    for(uint16_t i=0;i<9;i++)
    {
        dataShowCheckBox.append(new QCheckBox(this));   /* 添加数据显示确认标签 */
        dataShowCheckBox.constLast()->setText("DATA" + QString::number(i));
        dataShowCheckBox.constLast()->resize(80,20);
        dataShowCheckBox.constLast()->move(this->width()-80,i*20);
        dataShowCheckBox.constLast()->show();
    }
}

void waveWidget::readImuDataButton_clicked_slot()
{
    readImuDataFlag = 1;
    readImuDataButton->setDisabled(true);
}

void waveWidget::pauseWaveButtonButton_clicked_slot()
{
    if(pauseWaveButton->text() == "暂停波形")
    {
        pauseWaveDataFlag = 1;
        pauseWaveButton->setText("开始波形");
    }
    else if(pauseWaveButton->text() == "开始波形")
    {
        pauseWaveDataFlag = 0;
        pauseWaveButton->setText("暂停波形");
    }
}

void waveWidget::imuDataDeal_slot(_imuDataStruct imuData)
{
    static long long x = 0;
    if(0 == pauseWaveDataFlag)
    {
        static int32_t readDataCount = 0;
        if(1 == readImuDataFlag && readDataCount < 4000)
        {
            dataGyrox.append(imuData.accel_x);
            dataGyroy.append(imuData.accel_x_filter);
            dataGyroz.append(imuData.accel_y_filter);
            readDataCount++;
        }
        else if(readDataCount >= 4000)
        {
            QFile file("data.txt");
            file.open(QIODevice::WriteOnly | QIODevice::Text);/* 覆盖写入 */
            QTextStream stream(&file);

            for(uint32_t i=0;i<4000;i++)
            {
                QString dataLine;
                if(i<dataGyrox.size())
                {
                    dataLine += QString::number(dataGyrox.at(i)) + " ";
                }
                else
                {
                    dataLine += QString::number(0) + " ";
                }

                if(i<dataGyroy.size())
                {
                    dataLine += QString::number(dataGyroy.at(i)) + " ";
                }
                else
                {
                    dataLine += QString::number(0) + " ";
                }

                if(i<dataGyroz.size())
                {
                    dataLine += QString::number(dataGyroz.at(i)) + " ";
                }
                else
                {
                    dataLine += QString::number(0) + " ";
                }

                stream << dataLine << Qt::endl;
            }
            file.close();

            readImuDataFlag = 0;
            readDataCount = 0;
            readImuDataButton->setDisabled(false);
            qDebug() << "save done";
        }

        if(dataShowCheckBox.at(0)->checkState())
            waveShowPlot->graph(0)->addData(x, imuData.gyro_x_deg);
        if(dataShowCheckBox.at(1)->checkState())
            waveShowPlot->graph(1)->addData(x, imuData.gyro_y_deg);
        if(dataShowCheckBox.at(2)->checkState())
            waveShowPlot->graph(2)->addData(x, imuData.gyro_z_deg);
        // if(dataShowCheckBox.at(3)->checkState())
        //     waveShowPlot->graph(3)->addData(x, imuData.gyro_y_filter);
//        if(dataShowCheckBox.at(4)->checkState())
//            waveShowPlot->graph(4)->addData(x, imuData.gyro_z);
//        if(dataShowCheckBox.at(5)->checkState())
//            waveShowPlot->graph(5)->addData(x, imuData.gyro_z_filter);

        if(dataShowCheckBox.at(3)->checkState())
            waveShowPlot->graph(3)->addData(x, imuData.mag_x);
        if(dataShowCheckBox.at(4)->checkState())
            waveShowPlot->graph(4)->addData(x, imuData.mag_y);
        if(dataShowCheckBox.at(5)->checkState())
            waveShowPlot->graph(5)->addData(x, imuData.mag_z);

        if(dataShowCheckBox.at(6)->checkState())
            waveShowPlot->graph(6)->addData(x, imuData.accel_x);
        if(dataShowCheckBox.at(7)->checkState())
            waveShowPlot->graph(7)->addData(x, imuData.accel_y);
        if(dataShowCheckBox.at(8)->checkState())
            waveShowPlot->graph(8)->addData(x, imuData.accel_z);

        waveShowPlot->xAxis->setRange(x, 4000, Qt::AlignRight);
        waveShowPlot->yAxis->rescale();
        x++;
        if(0 == x%SHOW_RANGE)
        {
            if(dataShowCheckBox.at(0)->checkState())
                waveShowPlot->graph(0)->data()->removeBefore(x-SHOW_RANGE);
            if(dataShowCheckBox.at(1)->checkState())
                waveShowPlot->graph(1)->data()->removeBefore(x-SHOW_RANGE);
            if(dataShowCheckBox.at(2)->checkState())
                waveShowPlot->graph(2)->data()->removeBefore(x-SHOW_RANGE);
            if(dataShowCheckBox.at(3)->checkState())
                waveShowPlot->graph(3)->data()->removeBefore(x-SHOW_RANGE);
            if(dataShowCheckBox.at(4)->checkState())
                waveShowPlot->graph(4)->data()->removeBefore(x-SHOW_RANGE);
            if(dataShowCheckBox.at(5)->checkState())
                waveShowPlot->graph(5)->data()->removeBefore(x-SHOW_RANGE);
            if(dataShowCheckBox.at(6)->checkState())
                waveShowPlot->graph(6)->data()->removeBefore(x-SHOW_RANGE);
            if(dataShowCheckBox.at(7)->checkState())
                waveShowPlot->graph(7)->data()->removeBefore(x-SHOW_RANGE);
            if(dataShowCheckBox.at(8)->checkState())
                waveShowPlot->graph(8)->data()->removeBefore(x-SHOW_RANGE);

            waveShowPlot->xAxis->setRange(x, SHOW_RANGE, Qt::AlignRight);
        }
    }

}

void waveWidget::xAxisScrollBar_slot(int xAxisScrollBarValue)
{
    qDebug() << xAxisScrollBarValue;
}

void waveWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    waveShowPlot->resize(this->width()-80,this->height()-64);
    readImuDataButton->move(0,this->height()-32);
    pauseWaveButton->move(readImuDataButton->pos().x()+readImuDataButton->width(),this->height()-32);
    xAxisScrollBar->setGeometry(0, this->height()-64, this->width()-80, 32);
    for(uint16_t i=0;i<dataShowCheckBox.size();i++)
    {
        dataShowCheckBox.at(i)->move(this->width()-80,i*20);
    }
}
