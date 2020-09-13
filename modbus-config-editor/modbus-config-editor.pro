QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    modbusconfigeditorcontroller.cpp \
    modbusconfigeditormainwindow.cpp \
    modbusconfigmodel.cpp \
    modbusentities.cpp \
    serializer.cpp \
    serializerhelper.cpp \
    settingsmodel.cpp \
    utils.cpp \
    widgets/modbusdevicesettingswidget.cpp \
    widgets/registeraddresseditwidget.cpp \
    widgets/sensormapwidget.cpp \
    widgets/sensorsettingswidget.cpp \
    widgets/upaksettingswidget.cpp

HEADERS += \
    modbusconfigeditorcontroller.h \
    modbusconfigeditormainwindow.h \
    modbusconfigmodel.h \
    modbusentities.h \
    serializer.h \
    serializerhelper.h \
    settingsmodel.h \
    utils.h \
    widgets/modbusdevicesettingswidget.h \
    widgets/registeraddresseditwidget.h \
    widgets/sensormapwidget.h \
    widgets/sensorsettingswidget.h \
    widgets/upaksettingswidget.h

FORMS += \
    modbusconfigeditormainwindow.ui \
    widgets/modbusdevicesettingswidget.ui \
    widgets/registeraddresseditwidget.ui \
    widgets/sensormapwidget.ui \
    widgets/sensorsettingswidget.ui \
    widgets/upaksettingswidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
