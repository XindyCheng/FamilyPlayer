#ifndef QVIDEOWIDGET_P_H
#define QVIDEOWIDGET_P_H

#include <QVideoWidget>
#include <QKeyEvent>
#include <QTimer>

class QVideoWidget_p : public QVideoWidget
{
    Q_OBJECT
public:
    explicit QVideoWidget_p(QWidget *parent = nullptr);

signals:
    void videomouseClick();
    void videosetFullScreen();
    void videoshrink();
    void videoclearFocus();

private slots:
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseClick();

private:
    QTimer *timer;

};

#endif // QVIDEOWIDGET_P_H
