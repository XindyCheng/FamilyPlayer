#include "qwidget_p.h"

QWidget_p::QWidget_p(QWidget *parent)
    : QWidget{parent}
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setMouseTracking(true);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(mouseClick()));

#ifndef Q_OS_ANDROID // QTBUG-95723
    setAttribute(Qt::WA_OpaquePaintEvent);
#endif

}

void QWidget_p::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Escape) && isFullScreen()) {
        emit labelshrink();
    }
}

void QWidget_p::mouseDoubleClickEvent(QMouseEvent *event)
{
    timer->stop();
    if(isFullScreen())
    {
        emit labelshrink();
    }
    else
    {
        emit labelsetFullScreen();
    }
    event->accept();
}

void QWidget_p::mousePressEvent(QMouseEvent *event)
{
    emit labelclearFocus();
    timer->start(300);
    event->accept();
}

void QWidget_p::mouseClick()
{
    timer->stop();
    emit labelmouseClick();
}

