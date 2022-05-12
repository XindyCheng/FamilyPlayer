#include "qvideowidget_p.h"

QVideoWidget_p::QVideoWidget_p(QWidget *parent)
    : QVideoWidget(parent)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setMouseTracking(true);
    installEventFilter(this);

#ifndef Q_OS_ANDROID // QTBUG-95723
    setAttribute(Qt::WA_OpaquePaintEvent);
#endif
}

void QVideoWidget_p::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Escape) && isFullScreen()) {
        setFullScreen(false);
    }
}

void QVideoWidget_p::mouseDoubleClickEvent(QMouseEvent *event)
{
    setFullScreen(!isFullScreen());
    event->accept();
}

void QVideoWidget_p::mouseMoveEvent(QMouseEvent *event)
{
    emit mousemove();
    qDebug()<<"m1";
}

bool QVideoWidget_p::eventFilter(QObject *o, QEvent *e)
{
    if ((o == this) && (e->type() == QEvent::HoverMove))
    {
            emit mousemove();
            qDebug()<<"m2";
            return true;
    }
    return QVideoWidget::eventFilter(o,e);
}


