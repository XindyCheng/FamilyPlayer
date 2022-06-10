# QT       += core gui
QT   += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG(debug,debug|release)
{
    DESTDIR = $$PWD/bin/debug
}
CONFIG(release,debug|release)
{
    DESTDIR = $$PWD/bin/release
}
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialog.cpp \
    framelessdialog.cpp \
    framelessmainwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    player.cpp \
    playlist.cpp \
    qvideowidget_p.cpp \
    control.cpp \
    qwidget_p.cpp

HEADERS += \
    dialog.h \
    framelessdialog.h \
    framelessmainwindow.h \
    mainwindow.h \
    player.h \
    playlist.h \
    qvideowidget_p.h \
    control.h \
    qwidget_p.h

FORMS += \
    dialog.ui \
    mainwindow.ui \
    control.ui

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
    images/player.png \
    images/volume-high.PNG \
    images/volume-low.PNG \
    images/volume-off.PNG

RC_FILE += myapp.rc
