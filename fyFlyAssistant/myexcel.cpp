#include "myexcel.h"


myexcel::myexcel(QObject *parent)
    : QObject{parent}
{
    excel = new QAxObject("Excel.Application");
    if (excel->isNull()) {
        qDebug() << "Excel is not installed or could not be started";
    }
}

void myexcel::saveData(QList<float> data1, QList<float> data2, QList<float> data3)
{
    // 创建一个新的工作簿
    workbooks = excel->querySubObject("Workbooks");
    workbooks->dynamicCall("Add()");
    workbook = excel->querySubObject("ActiveWorkBook");

    // 获取第一个表格
    QAxObject *sheets = workbook->querySubObject("Sheets");
    QAxObject *sheet = sheets->querySubObject("Item(int)", 1);

    // 写入三列数据，每列包含元素
    int rowCount = qMax(data1.size(), qMax(data2.size(), data3.size()));
    for (int i = 0; i < rowCount; ++i) {
        if (i < data1.size()) {
            QAxObject *cellA = sheet->querySubObject("Cells(int,int)", i + 1, 1);
            cellA->setProperty("Value", data1.at(i));
            delete cellA;
        }
        if (i < data2.size()) {
            QAxObject *cellB = sheet->querySubObject("Cells(int,int)", i + 1, 2);
            cellB->setProperty("Value", data2.at(i));
            delete cellB;
        }
        if (i < data3.size()) {
            QAxObject *cellC = sheet->querySubObject("Cells(int,int)", i + 1, 3);
            cellC->setProperty("Value", data3.at(i));
            delete cellC;
        }
    }

    // 保存文件到当前程序运行路径下
    QString currentPath = QDir::currentPath();
    QString filePath = currentPath + "/output.xlsx";

    QVariant result = workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(filePath));
    if (result.isValid()) {
        qDebug() << "File saved successfully";
    } else {
        qDebug() << "Failed to save the file. Error: " << result.toString();
    }

    // 关闭Excel应用程序
    excel->dynamicCall("Quit()");
}

