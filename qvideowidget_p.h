#ifndef QVIDEOWIDGET_P_H
#define QVIDEOWIDGET_P_H

#include <QVideoWidget>
#include <QKeyEvent>

class QVideoWidget_p : public QVideoWidget
{
    Q_OBJECT
public:
    explicit QVideoWidget_p(QWidget *parent = nullptr);

signals:
    void mousemove();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e) override;

};

#endif // QVIDEOWIDGET_P_H
