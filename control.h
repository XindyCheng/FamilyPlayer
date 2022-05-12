#ifndef CONTROL_H
#define CONTROL_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class Control;
}

class Control : public QDialog
{
    Q_OBJECT

public:
    explicit Control(QWidget *parent = nullptr);
    ~Control();

private slots:
    void onTimeout();
    //void mousemove();
    void moveTime();

private:
    Ui::Control *ui;
    QTimer *timer;
    QTimer *move;

    int mouse_x;
    int mouse_y;
};

#endif // CONTROL_H
