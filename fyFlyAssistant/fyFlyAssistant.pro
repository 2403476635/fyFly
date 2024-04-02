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

DEFINES += QCUSTOMPLOT_USE_OPENGL

SOURCES += \
    ellipsoidfitwidget.cpp \
    flyboarddatashowwidget.cpp \
    loadModel/ModelWindow.cpp \
    loadModel/Q3DMesh.cpp \
    loadModel/Q3DModel.cpp \
    main.cpp \
    mycombobox.cpp \
    parametersettingwidget.cpp \
    qcustomplot.cpp \
    ringbufer.cpp \
    serialport.cpp \
    settingwidget.cpp \
    wavewidget.cpp \
    widget.cpp

HEADERS += \
    common.h \
    ellipsoidfitwidget.h \
    flyboarddatashowwidget.h \
    loadModel/ModelWindow.h \
    loadModel/Q3DMesh.h \
    loadModel/Q3DModel.h \
    mycombobox.h \
    parametersettingwidget.h \
    qcustomplot.h \
    ringbufer.h \
    serialport.h \
    settingwidget.h \
    stable.h \
    wavewidget.h \
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

win32: LIBS += -L$$PWD/3dparty/assimp/lib/ -llibassimp.dll -lopengl32 -lglu32

INCLUDEPATH += $$PWD/3dparty/assimp
INCLUDEPATH += $$PWD/3dparty/eigen-3.4.0

DEPENDPATH += $$PWD/3dparty/assimp

QMAKE_CXXFLAGS += "-Wa,-mbig-obj"   #解决源码编译提示太大的报错

# Use Precompiled headers (PCH) #预编译头加快编译速度
CONFIG += precompile_header
PRECOMPILED_HEADER = stable.h
