#include "ellipsoidfitwidget.h"

using namespace Eigen;

ellipsoidfitWidget::ellipsoidfitWidget(QWidget *parent)
    : QWidget{parent}
{

}

void ellipsoidfitWidget::windowInit()
{
    this->hide();
    for(uint8_t i=0;i<12;i++)
    {
        showDataLabel.append(new QLabel(this));
        showDataLabel.at(i)->setText("NaN");
        showDataLabel.at(i)->setGeometry((i%3)*90,(i/3)*30,80,20);    /* 设置坐标 */
        showDataLabel.at(i)->show();
    }

    sendCorrectParameterButton = new QPushButton(this);
    sendCorrectParameterButton->setText("发送参数");
    sendCorrectParameterButton->setGeometry(800-80*1, 600-32, 80, 32);
    connect(sendCorrectParameterButton, &QPushButton::clicked,this,&ellipsoidfitWidget::sendCorrectParameterButton_clicked_slot);

    readCorrectParameterButton = new QPushButton(this);
    readCorrectParameterButton->setText("读取参数");
    readCorrectParameterButton->setGeometry(800-80*2, 600-32, 80, 32);
    connect(readCorrectParameterButton, &QPushButton::clicked,this,&ellipsoidfitWidget::readCorrectParameterButton_clicked_slot);

    calculateParameterButton = new QPushButton(this);
    calculateParameterButton->setText("读取数据");
    calculateParameterButton->setGeometry(800-80*3, 600-32, 80, 32);
    connect(calculateParameterButton, &QPushButton::clicked,this,&ellipsoidfitWidget::calculateParameterButton_clicked_slot);

    clearDataShowButton = new QPushButton(this);
    clearDataShowButton->setText("清除显示数据");
    clearDataShowButton->setGeometry(800-80*4, 600-32, 80, 32);
    connect(clearDataShowButton, &QPushButton::clicked,this,&ellipsoidfitWidget::clearDataShowButton_clicked_slot);

    setCalibrationButton = new QPushButton(this);
    setCalibrationButton->setText("设置校准模式");
    setCalibrationButton->setGeometry(800-80*2, 600-32*2, 160, 32);
    connect(setCalibrationButton, &QPushButton::clicked,this,&ellipsoidfitWidget::setCalibrationButtonButton_clicked_slot);

    mChooseCalibrationModeComboBox = new QComboBox(this);
    mChooseCalibrationModeComboBox->setGeometry(800-80*1-20,600-32*3,100,32);
    mChooseCalibrationModeComboBox->addItem("陀螺仪校准");
    mChooseCalibrationModeComboBox->addItem("加速度计校准");
    mChooseCalibrationModeComboBox->addItem("磁力计校准");

    mReadDataCountLineEdit = new QLineEdit(this);
    mReadDataCountLineEdit->setText("4000");
    mReadDataCountLineEdit->setGeometry(800-80*1-20,600-32*4,100,32);

    mProgressBar = new QProgressBar(this);
    mProgressBar->setGeometry(0,600-32,160,32);
    mProgressBar->setOrientation(Qt::Horizontal);  // 水平方向
    mProgressBar->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);  // 对齐方式

    label1 = new QLabel(this);
    label1->setGeometry(800-80*2,600-32*3,60,32);
    label1->setText("模式:");

    label2 = new QLabel(this);
    label2->setGeometry(800-80*2,600-32*4,80,32);
    label2->setText("数据量:");

    clearDataShowButton_clicked_slot();
}

void ellipsoidfitWidget::showFlyBoardCalibrationParameter_slot(QList<double> data)
{
    qDebug() << "showFlyBoardCalibrationParameter_slot";
    if(GYRO_CALIBRATION_MODE == nowCalibrationMode)
    {
        showDataLabel.at(0)->setText("X:" + QString::number(data.at(0)));
        showDataLabel.at(1)->setText("Y:" + QString::number(data.at(1)));
        showDataLabel.at(2)->setText("Z:" + QString::number(data.at(2)));
    }
    else
    {
        showDataLabel.at(0)->setText("X:" + QString::number(data.at(0)));
        showDataLabel.at(1)->setText("Y:" + QString::number(data.at(1)));
        showDataLabel.at(2)->setText("Z:" + QString::number(data.at(2)));
        for (uint8_t i=3;i<12;i++)
        {
           showDataLabel.at(i)->setText(QString::number(data.at(i)));
        }
    }
}

void ellipsoidfitWidget::originalSensorForCalibrationData_slot(_originalSensorForCalibrationData data)
{
    if(isRecordFlag)
    {
        qDebug()  << originalSensorData.count();
        if(originalSensorData.count() < readDataCount)     /*  */
        {
            originalSensorData.append(data);                /* 记录数据 */
        }
        else if(originalSensorData.count() >= readDataCount) /* 接收数据满足设定的量 */
        {
            isRecordFlag = false;
            calculateParameterButton->setDisabled(false);
        }

        mProgressBar->setValue(originalSensorData.count());               // 当前进度
        double dProgress =    (mProgressBar->value() - mProgressBar->minimum()) * 100.0
                            / (mProgressBar->maximum() - mProgressBar->minimum()); // 百分比计算公式
        mProgressBar->setFormat(QString::fromLocal8Bit("%1%").arg(QString::number(dProgress, 'f', 1))) ;
    }
}

void ellipsoidfitWidget::calculateParameterButton_clicked_slot()
{
    if("读取数据" == calculateParameterButton->text())
    {
        isRecordFlag = true;
        calculateParameterButton->setDisabled(true);
        calculateParameterButton->setText("计算校准参数");
    }
    else if("计算校准参数" == calculateParameterButton->text())
    {
        ellipsoidData.clear();
        calculateParameterButton->setText("读取数据");
        if(!originalSensorData.isEmpty())
        {
            if(GYRO_CALIBRATION_MODE == nowCalibrationMode) /* 陀螺仪校准 */
            {
                qDebug() << "GYRO_CALIBRATION_MODE == nowCalibrationMode";
                /* 陀螺仪计算零偏 */
                double x = 0.0f,y = 0.0f,z = 0.0f;
                for(uint32_t i=0;i<originalSensorData.count();i++)
                {
                    x += originalSensorData.at(i).dataX;
                    y += originalSensorData.at(i).dataY;
                    z += originalSensorData.at(i).dataZ;
                }
                x /= originalSensorData.count();
                y /= originalSensorData.count();
                z /= originalSensorData.count();

                ellipsoidData.append(x);
                ellipsoidData.append(y);
                ellipsoidData.append(z);
                qDebug() << "x = " << x << "y = " << y << "z = " << z;
            }
            else
            {
                /* 计算矫正参数 */
                ellipsoidFit(originalSensorData,originalSensorData.count());
                /* 显示 */
                showDataLabel.at(0)->setText("X:" + QString::number(ellipsoidData.at(0)));
                showDataLabel.at(1)->setText("Y:" + QString::number(ellipsoidData.at(1)));
                showDataLabel.at(2)->setText("Z:" + QString::number(ellipsoidData.at(2)));
                for (uint8_t i=3;i<12;i++)
                {
                   showDataLabel.at(i)->setText(QString::number(ellipsoidData.at(i)));
                }
            }

            mChooseCalibrationModeComboBox->setDisabled(false);
            mReadDataCountLineEdit->setDisabled(false);

            isEllipsoidFitDone = true;

            originalSensorData.clear();                             /* 清除上次的数据 */
        }
    }
}

void ellipsoidfitWidget::readCorrectParameterButton_clicked_slot()
{
    QByteArray tempData;
    tempData.resize(1);
    tempData[0] = nowCalibrationMode;
    emit correctSend_signal(setSerialPortStringDataFormat(0xAA, PC, CMD_READ_CORRECT_PARAMETER, tempData));
}

void ellipsoidfitWidget::sendCorrectParameterButton_clicked_slot()
{
    if(!isEllipsoidFitDone)return ;/* 判断计算是否完成 */
    if(GYRO_CALIBRATION_MODE == nowCalibrationMode)/* 判断当前的校准模式 */
    {
        QByteArray tempData;
        int32_t tempnumber = 0;
        uint8_t cnt = 0;
        tempData.resize(12);
        for(uint8_t i=0;i<3;i++)
        {
            tempnumber = ellipsoidData.at(i)*1000000;
            tempData[cnt++] = tempnumber;
            tempData[cnt++] = tempnumber>>8;
            tempData[cnt++] = tempnumber>>16;
            tempData[cnt++] = tempnumber>>24;
        }
        qDebug() << "0 = " << ellipsoidData.at(0) << "1 = " << ellipsoidData.at(1) << "2 = " << ellipsoidData.at(2);
        emit correctSend_signal(setSerialPortStringDataFormat(0xAA, PC, CMD_SEND_CORRECT_PARAMETER, tempData));
    }
    else
    {
        QByteArray tempData;
        int32_t tempnumber = 0;
        uint8_t cnt = 0;
        tempData.resize(48);
        for(uint8_t i=0;i<12;i++)
        {
            tempnumber = ellipsoidData.at(i)*1000000;
            tempData[cnt++] = tempnumber;
            tempData[cnt++] = tempnumber>>8;
            tempData[cnt++] = tempnumber>>16;
            tempData[cnt++] = tempnumber>>24;
        }
        emit correctSend_signal(setSerialPortStringDataFormat(0xAA, PC, CMD_SEND_CORRECT_PARAMETER, tempData));
    }
}

void ellipsoidfitWidget::clearDataShowButton_clicked_slot()
{
    showDataLabel.at(0)->setText("X:NaN");
    showDataLabel.at(1)->setText("Y:NaN");
    showDataLabel.at(2)->setText("Z:NaN");
    for (uint8_t i=3;i<12;i++)
    {
       showDataLabel.at(i)->setText("NaN");
    }
}

void ellipsoidfitWidget::setCalibrationButtonButton_clicked_slot()
{
    if("设置校准模式" == setCalibrationButton->text())
    {
        QByteArray tempData;
        tempData.resize(1);
        if("陀螺仪校准" == mChooseCalibrationModeComboBox->currentText())
        {
            nowCalibrationMode = tempData[0] = GYRO_CALIBRATION_MODE;
        }
        else if("加速度计校准" == mChooseCalibrationModeComboBox->currentText())
        {
            nowCalibrationMode = tempData[0] = ACCEL_CALIBRATION_MODE;
        }
        else if("磁力计校准" == mChooseCalibrationModeComboBox->currentText())
        {
            nowCalibrationMode = tempData[0] = MAG_CALIBRATION_MODE;
        }
        emit correctSend_signal(setSerialPortStringDataFormat(0xAA, PC, CMD_SET_CALIBRATION_MODE, tempData));

        readDataCount = mReadDataCountLineEdit->text().toUInt();    /* 获取要记录的校准数据的个数 */

        mProgressBar->setMinimum(0);                // 最小值
        mProgressBar->setMaximum(readDataCount);    // 最大值
        mProgressBar->setValue(0);                  // 当前进度

        setCalibrationButton->setText("退出校准模式");
    }
    else if("退出校准模式" == setCalibrationButton->text())
    {
        QByteArray tempData;
        tempData.resize(1);
        tempData[0] = 0;
        emit correctSend_signal(setSerialPortStringDataFormat(0xAA, PC, CMD_SET_CALIBRATION_MODE, tempData));

        mChooseCalibrationModeComboBox->setDisabled(false);
        mReadDataCountLineEdit->setDisabled(false);

        originalSensorData.clear();                             /* 清除上次的数据 */

        setCalibrationButton->setText("设置校准模式");
    }
}

void ellipsoidfitWidget::ellipsoidFit(const char *filename_in, const char *filename_out, int mag_data_counter)
{
    FILE * fp;
    double mag_x, mag_y, mag_z;

    MatrixXd mat_D(mag_data_counter, 9);
    MatrixXd mat_DT;

    fp = fopen(filename_in, "r");

    if (fp == NULL)
    {
        qDebug() << "Error opening %s\n." << filename_in;
        exit(1);
    }

    for (int i = 0; i < mag_data_counter; i++)
    {
        fscanf(fp, "%lf %lf %lf\n", &mag_x, &mag_y, &mag_z);
        mat_D(i, 0) = mag_x * mag_x;
        mat_D(i, 1) = mag_y * mag_y;
        mat_D(i, 2) = mag_z * mag_z;
        mat_D(i, 3) = 2 * mag_x * mag_y;
        mat_D(i, 4) = 2 * mag_x * mag_z;
        mat_D(i, 5) = 2 * mag_y * mag_z;
        mat_D(i, 6) = 2 * mag_x;
        mat_D(i, 7) = 2 * mag_y;
        mat_D(i, 8) = 2 * mag_z;
    }

    fclose(fp);

    mat_DT = mat_D.transpose();

    MatrixXd mat_Ones = MatrixXd::Ones(mag_data_counter, 1);

    MatrixXd mat_Result =  (mat_DT * mat_D).inverse() * (mat_DT * mat_Ones);

    Matrix<double, 4, 4>  mat_A_4x4;

    mat_A_4x4(0, 0) = mat_Result(0, 0);
    mat_A_4x4(0, 1) = mat_Result(3, 0);
    mat_A_4x4(0, 2) = mat_Result(4, 0);
    mat_A_4x4(0, 3) = mat_Result(6, 0);

    mat_A_4x4(1, 0) = mat_Result(3, 0);
    mat_A_4x4(1, 1) = mat_Result(1, 0);
    mat_A_4x4(1, 2) = mat_Result(5, 0);
    mat_A_4x4(1, 3) = mat_Result(7, 0);

    mat_A_4x4(2, 0) = mat_Result(4, 0);
    mat_A_4x4(2, 1) = mat_Result(5, 0);
    mat_A_4x4(2, 2) = mat_Result(2, 0);
    mat_A_4x4(2, 3) = mat_Result(8, 0);

    mat_A_4x4(3, 0) = mat_Result(6, 0);
    mat_A_4x4(3, 1) = mat_Result(7, 0);
    mat_A_4x4(3, 2) = mat_Result(8, 0);
    mat_A_4x4(3, 3) = -1.0;

    MatrixXd mat_Center = -((mat_A_4x4.block(0, 0, 3, 3)).inverse() * mat_Result.block(6, 0, 3, 1));

    Matrix<double, 4, 4>  mat_T_4x4;
    mat_T_4x4.setIdentity();
    mat_T_4x4.block(3, 0, 1, 3) = mat_Center.transpose();

    MatrixXd mat_R = mat_T_4x4 * mat_A_4x4 * mat_T_4x4.transpose();

    EigenSolver<MatrixXd> eig(mat_R.block(0, 0, 3, 3) / -mat_R(3, 3));
    //mat_T_4x4(3,0)=mat_Center()
    MatrixXd mat_Eigval(3, 1) ;
    MatrixXd mat_Evecs(3, 3) ;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            mat_Evecs(i, j) = (eig.eigenvectors())(i, j).real();
        }
    }

    mat_Eigval(0, 0) = (eig.eigenvalues())(0, 0).real();
    mat_Eigval(1, 0) = (eig.eigenvalues())(1, 0).real();
    mat_Eigval(2, 0) = (eig.eigenvalues())(2, 0).real();
    MatrixXd mat_Radii = (1.0 / mat_Eigval.array()).cwiseSqrt();
    MatrixXd mat_Scale = MatrixXd::Identity(3, 3) ;
    mat_Scale(0, 0) = mat_Radii(0, 0);
    mat_Scale(1, 1) = mat_Radii(1, 0);
    mat_Scale(2, 2) = mat_Radii(2, 0);
    double min_Radii = mat_Radii.minCoeff();

    mat_Scale = mat_Scale.inverse().array() * min_Radii;
    MatrixXd mat_Correct = mat_Evecs * mat_Scale * mat_Evecs.transpose();

    ellipsoidData.append(mat_Center(0));
    ellipsoidData.append(mat_Center(1));
    ellipsoidData.append(mat_Center(2));

    ellipsoidData.append(mat_Correct(0));
    ellipsoidData.append(mat_Correct(1));
    ellipsoidData.append(mat_Correct(2));
    ellipsoidData.append(mat_Correct(3));
    ellipsoidData.append(mat_Correct(4));
    ellipsoidData.append(mat_Correct(5));
    ellipsoidData.append(mat_Correct(6));
    ellipsoidData.append(mat_Correct(7));
    ellipsoidData.append(mat_Correct(8));

    qDebug() << "The Ellipsoid center is:" << "\n" << mat_Center(0) << mat_Center(1) << mat_Center(2) << "\n";
    qDebug() << "The Ellipsoid radii is:" << "\n" << mat_Radii(0,0) << mat_Radii(1,0) << mat_Radii(2,0) << "\n";
    qDebug() << "The scale matrix  is:" << "\n" << mat_Scale(0,0) << mat_Scale(1,1) << mat_Scale(2,2) << "\n";
    qDebug() << "The correct matrix  is:" << "\n"   << mat_Correct(0) << mat_Correct(1) << mat_Correct(2) << "\n"
                                                    << mat_Correct(3) << mat_Correct(4) << mat_Correct(5) << "\n"
                                                    << mat_Correct(6) << mat_Correct(7) << mat_Correct(8) << "\n";

    FILE *correction_file = fopen(filename_out, "w");
    if (correction_file == NULL)
    {
        printf("Error opening %s\n", filename_out);
        exit(1);
    }

    fprintf(correction_file, "%f %f %f %f %f %f %f %f %f %f %f %f\n",
    mat_Center(0), mat_Center(1), mat_Center(2),
    mat_Correct(0,0), mat_Correct(0,1), mat_Correct(0,2),
    mat_Correct(1,0), mat_Correct(1,1), mat_Correct(1,2),
    mat_Correct(2,0), mat_Correct(2,1), mat_Correct(2,2));

    fclose(correction_file);
}

void ellipsoidfitWidget::ellipsoidFit(QList<_originalSensorForCalibrationData> data, int mag_data_counter)
{
    double mag_x, mag_y, mag_z;

    MatrixXd mat_D(mag_data_counter, 9);
    MatrixXd mat_DT;

    for (int i = 0; i < mag_data_counter; i++)
    {
        mag_x = data.at(i).dataX;
        mag_y = data.at(i).dataY;
        mag_z = data.at(i).dataZ;
        qDebug() << data.at(i).dataX << ":" << data.at(i).dataY << ":" << data.at(i).dataZ;
        mat_D(i, 0) = mag_x * mag_x;
        mat_D(i, 1) = mag_y * mag_y;
        mat_D(i, 2) = mag_z * mag_z;
        mat_D(i, 3) = 2 * mag_x * mag_y;
        mat_D(i, 4) = 2 * mag_x * mag_z;
        mat_D(i, 5) = 2 * mag_y * mag_z;
        mat_D(i, 6) = 2 * mag_x;
        mat_D(i, 7) = 2 * mag_y;
        mat_D(i, 8) = 2 * mag_z;
    }

    mat_DT = mat_D.transpose();

    MatrixXd mat_Ones = MatrixXd::Ones(mag_data_counter, 1);

    MatrixXd mat_Result =  (mat_DT * mat_D).inverse() * (mat_DT * mat_Ones);

    Matrix<double, 4, 4>  mat_A_4x4;

    mat_A_4x4(0, 0) = mat_Result(0, 0);
    mat_A_4x4(0, 1) = mat_Result(3, 0);
    mat_A_4x4(0, 2) = mat_Result(4, 0);
    mat_A_4x4(0, 3) = mat_Result(6, 0);

    mat_A_4x4(1, 0) = mat_Result(3, 0);
    mat_A_4x4(1, 1) = mat_Result(1, 0);
    mat_A_4x4(1, 2) = mat_Result(5, 0);
    mat_A_4x4(1, 3) = mat_Result(7, 0);

    mat_A_4x4(2, 0) = mat_Result(4, 0);
    mat_A_4x4(2, 1) = mat_Result(5, 0);
    mat_A_4x4(2, 2) = mat_Result(2, 0);
    mat_A_4x4(2, 3) = mat_Result(8, 0);

    mat_A_4x4(3, 0) = mat_Result(6, 0);
    mat_A_4x4(3, 1) = mat_Result(7, 0);
    mat_A_4x4(3, 2) = mat_Result(8, 0);
    mat_A_4x4(3, 3) = -1.0;

    MatrixXd mat_Center = -((mat_A_4x4.block(0, 0, 3, 3)).inverse() * mat_Result.block(6, 0, 3, 1));

    Matrix<double, 4, 4>  mat_T_4x4;
    mat_T_4x4.setIdentity();
    mat_T_4x4.block(3, 0, 1, 3) = mat_Center.transpose();

    MatrixXd mat_R = mat_T_4x4 * mat_A_4x4 * mat_T_4x4.transpose();

    EigenSolver<MatrixXd> eig(mat_R.block(0, 0, 3, 3) / -mat_R(3, 3));
    //mat_T_4x4(3,0)=mat_Center()
    MatrixXd mat_Eigval(3, 1) ;
    MatrixXd mat_Evecs(3, 3) ;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            mat_Evecs(i, j) = (eig.eigenvectors())(i, j).real();
        }
    }

    mat_Eigval(0, 0) = (eig.eigenvalues())(0, 0).real();
    mat_Eigval(1, 0) = (eig.eigenvalues())(1, 0).real();
    mat_Eigval(2, 0) = (eig.eigenvalues())(2, 0).real();
    MatrixXd mat_Radii = (1.0 / mat_Eigval.array()).cwiseSqrt();
    MatrixXd mat_Scale = MatrixXd::Identity(3, 3) ;
    mat_Scale(0, 0) = mat_Radii(0, 0);
    mat_Scale(1, 1) = mat_Radii(1, 0);
    mat_Scale(2, 2) = mat_Radii(2, 0);
    double min_Radii = mat_Radii.minCoeff();

    mat_Scale = mat_Scale.inverse().array() * min_Radii;
    MatrixXd mat_Correct = mat_Evecs * mat_Scale * mat_Evecs.transpose();

    ellipsoidData.append(mat_Center(0));
    ellipsoidData.append(mat_Center(1));
    ellipsoidData.append(mat_Center(2));

    ellipsoidData.append(mat_Correct(0));
    ellipsoidData.append(mat_Correct(1));
    ellipsoidData.append(mat_Correct(2));
    ellipsoidData.append(mat_Correct(3));
    ellipsoidData.append(mat_Correct(4));
    ellipsoidData.append(mat_Correct(5));
    ellipsoidData.append(mat_Correct(6));
    ellipsoidData.append(mat_Correct(7));
    ellipsoidData.append(mat_Correct(8));

    qDebug() << "The Ellipsoid center is:" << "\n" << mat_Center(0) << mat_Center(1) << mat_Center(2) << "\n";
    qDebug() << "The Ellipsoid radii is:" << "\n" << mat_Radii(0,0) << mat_Radii(1,0) << mat_Radii(2,0) << "\n";
    qDebug() << "The scale matrix  is:" << "\n" << mat_Scale(0,0) << mat_Scale(1,1) << mat_Scale(2,2) << "\n";
    qDebug() << "The correct matrix  is:" << "\n"   << mat_Correct(0) << mat_Correct(1) << mat_Correct(2) << "\n"
                                                    << mat_Correct(3) << mat_Correct(4) << mat_Correct(5) << "\n"
                                                    << mat_Correct(6) << mat_Correct(7) << mat_Correct(8) << "\n";

}

QByteArray ellipsoidfitWidget::setSerialPortStringDataFormat(uint8_t frameHead, uint8_t frameAddress, uint8_t frameID, QByteArray data)
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

