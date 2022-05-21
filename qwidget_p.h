#ifndef QWIDGET_P_H
#define QWIDGET_P_H

#include <QWidget>
#include <QKeyEvent>
#include <QTimer>

class QWidget_p : public QWidget
{
    Q_OBJECT
public:
    explicit QWidget_p(QWidget *parent = nullptr);

signals:
    void labelmouseClick();
    void labelsetFullScreen();
    void labelshrink();
    void labelclearFocus();

private slots:
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseClick();

private:
    QTimer *timer;

};

#endif // QWIDGET_P_H
