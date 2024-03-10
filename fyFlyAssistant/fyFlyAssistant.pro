QT       += core gui
QT       += opengl
QT       += serialport   #加入串口模块
QT       += openglwidgets
QT       += axcontainer
QT       += printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    flyboarddatashowwidget.cpp \
    loadModel/ModelWindow.cpp \
    loadModel/Q3DMesh.cpp \
    loadModel/Q3DModel.cpp \
    main.cpp \
    mycombobox.cpp \
    myexcel.cpp \
    qcustomplot.cpp \
    ringbufer.cpp \
    serialport.cpp \
    settingwidget.cpp \
    widget.cpp

HEADERS += \
    common.h \
    flyboarddatashowwidget.h \
    loadModel/ModelWindow.h \
    loadModel/Q3DMesh.h \
    loadModel/Q3DModel.h \
    mycombobox.h \
    myexcel.h \
    qcustomplot.h \
    ringbufer.h \
    serialport.h \
    settingwidget.h \
    widget.h

FORMS += \
    widget.ui

#添加程序的logo
RC_ICONS = icon\logo.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    flyModel.qrc \
    images.qrc \
    loadModel/shader.qrc

win32: LIBS += -L$$PWD/3dparty/assimp/lib/ -llibassimp.dll
INCLUDEPATH += $$PWD/3dparty/assimp
DEPENDPATH += $$PWD/3dparty/assimp

