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
    sendCorrectParameterButton->resize(80,32);
    sendCorrectParameterButton->setText("发送参数");
    sendCorrectParameterButton->move(800-80,600-32);
    connect(sendCorrectParameterButton, &QPushButton::clicked,this,&ellipsoidfitWidget::sendCorrectParameterButton_clicked_slot);

    readCorrectParameterButton = new QPushButton(this);
    readCorrectParameterButton->resize(80,32);
    readCorrectParameterButton->setText("读取参数");
    readCorrectParameterButton->move(800-80*2,600-32);
    connect(readCorrectParameterButton, &QPushButton::clicked,this,&ellipsoidfitWidget::readCorrectParameterButton_clicked_slot);

    openFileButton = new QPushButton(this);
    openFileButton->resize(80,32);
    openFileButton->setText("计算校正参数");
    openFileButton->move(800-80*3,600-32);
    connect(openFileButton, &QPushButton::clicked,this,&ellipsoidfitWidget::openFileButton_clicked_slot);

    clearDataShowButton = new QPushButton(this);
    clearDataShowButton->resize(80,32);
    clearDataShowButton->setText("清除显示数据");
    clearDataShowButton->move(800-80*4,600-32);
    connect(clearDataShowButton, &QPushButton::clicked,this,&ellipsoidfitWidget::clearDataShowButton_clicked_slot);

    mProgressBar = new QProgressBar(this);

    clearDataShowButton_clicked_slot();
}

void ellipsoidfitWidget::showFlyBoardCalibrationParameter_slot(QList<double> data)
{
    showDataLabel.at(0)->setText("X:" + QString::number(data.at(0)));
    showDataLabel.at(1)->setText("Y:" + QString::number(data.at(1)));
    showDataLabel.at(2)->setText("Z:" + QString::number(data.at(2)));
    for (uint8_t i=3;i<12;i++)
    {
       showDataLabel.at(i)->setText(QString::number(data.at(i)));
    }
    qDebug() << "showFlyBoardCalibrationParameter_slot";
}

void ellipsoidfitWidget::openFileButton_clicked_slot()
{
    QString defaultFolder = QDir::currentPath();
    QString filePath = QFileDialog::getOpenFileName(
       nullptr,
       "选择文件",
       defaultFolder,
       "文本文件 (*.txt)"
    );
    if (!filePath.isEmpty())
    {
        qDebug() << "选择的文件路径：" << filePath; // 用户选择了一个.txt文件，可以在这里对路径进行处理
        QString outPath = defaultFolder + "/outputdata.txt";
        ellipsoidData.clear();
        /* 计算TXT文件的数据行数 */
        QFile file(filePath);
        file.open(QFile::ReadOnly);
        int count=0;
        while(!file.atEnd())
        {
           file.readLine();
           count++;
        }
        /* 计算矫正参数 */
        ellipsoidFit(filePath.toLocal8Bit().constData(),outPath.toLocal8Bit().constData(),count);
        /* 显示 */
        showDataLabel.at(0)->setText("X:" + QString::number(ellipsoidData.at(0)));
        showDataLabel.at(1)->setText("Y:" + QString::number(ellipsoidData.at(1)));
        showDataLabel.at(2)->setText("Z:" + QString::number(ellipsoidData.at(2)));
        for (uint8_t i=3;i<12;i++)
        {
           showDataLabel.at(i)->setText(QString::number(ellipsoidData.at(i)));
        }
        isEllipsoidFitDone = true;
    }
}

void ellipsoidfitWidget::readCorrectParameterButton_clicked_slot()
{
    emit correctSend_signal(setSerialPortStringDataFormat(0xAA, PC, CMD_READ_CORRECT_PARAMETER, 0));
}

void ellipsoidfitWidget::sendCorrectParameterButton_clicked_slot()
{
    if(!isEllipsoidFitDone)return ;
    QByteArray tempdata;
    int32_t tempnumber = 0;
    uint8_t cnt = 0;
    tempdata.resize(48);
    for(uint8_t i=0;i<12;i++)
    {
        tempnumber = ellipsoidData.at(i)*1000000;
        tempdata[cnt++] = tempnumber;
        tempdata[cnt++] = tempnumber>>8;
        tempdata[cnt++] = tempnumber>>16;
        tempdata[cnt++] = tempnumber>>24;
    }
    emit correctSend_signal(setSerialPortStringDataFormat(0xAA, PC, CMD_SEND_CORRECT_PARAMETER, tempdata));
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

