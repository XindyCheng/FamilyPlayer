# QT       += core gui
QT   += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qvideowidget_p.cpp

HEADERS += \
    mainwindow.h \
    qvideowidget_p.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    test1.qrc

DISTFILES += \
    images/button-fast-forward.PNG \
    images/button-next.PNG \
    images/button-pause.PNG \
    images/button-play.PNG \
    images/button-previous.PNG \
    images/button-rewind.PNG \
    images/button-stop.PNG \
    images/volume-high.PNG \
    images/volume-low.PNG \
    images/volume-off.PNG
