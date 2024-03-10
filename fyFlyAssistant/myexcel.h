#ifndef MYEXCEL_H
#define MYEXCEL_H

#include <QObject>
#include <QAxObject>
#include <QDir>
#include <QList>

class myexcel : public QObject
{
    Q_OBJECT
public:
    explicit myexcel(QObject *parent = nullptr);

    void saveData(QList<float> data1, QList<float> data2, QList<float> data3);
signals:
private:
    QAxObject *excel;
    QAxObject *workbooks;
    QAxObject *workbook;
};

#endif // MYEXCEL_H
