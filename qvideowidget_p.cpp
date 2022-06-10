#include "qvideowidget_p.h"

QVideoWidget_p::QVideoWidget_p(QWidget *parent)
    : QVideoWidget(parent)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setMouseTracking(true);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(mouseClick()));

#ifndef Q_OS_ANDROID // QTBUG-95723
    setAttribute(Qt::WA_OpaquePaintEvent);
#endif
}

void QVideoWidget_p::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Escape) && isFullScreen()) {
        emit videoshrink();
    }
}

void QVideoWidget_p::mouseDoubleClickEvent(QMouseEvent *event)
{
    timer->stop();
    if(isFullScreen())
    {
        emit videoshrink();
    }
    else
    {
        emit videosetFullScreen();
    }
    event->accept();
}

void QVideoWidget_p::mousePressEvent(QMouseEvent *event)
{
    emit videoclearFocus();
    timer->start(300);
    event->accept();
}

void QVideoWidget_p::mouseClick()
{
    timer->stop();
    emit videomouseClick();
}



