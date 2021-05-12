QT       += core gui network sql serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dbmanager.cpp \
    logcontainer.cpp \
    logutilities.cpp \
    main.cpp \
    mainwindow.cpp \
    measurements.cpp \
    qcustomplot.cpp \
    seriallistener.cpp \
    serverinstance.cpp \
    udplistener.cpp

HEADERS += \
    Global.h \
    dbmanager.h \
    logcontainer.h \
    logutilities.h \
    mainwindow.h \
    measurements.h \
    qcustomplot.h \
    seriallistener.h \
    serverinstance.h \
    udplistener.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Database.db \
    DatabaseCopy.db
