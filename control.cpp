#include "control.h"
#include "ui_control.h"

Control::Control(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Control)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);

    timer = new QTimer;
    move = new QTimer;

    mouse_x=QCursor().pos().x();
    mouse_y=QCursor().pos().y();

    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect(move, SIGNAL(timeout()), this, SLOT(moveTime()));

    timer->start(5000);
    move->start(100);

    //setMouseTracking(true);
    //parent->setMouseTracking(true);


    connect(parent, SIGNAL(mousemove()), this, SLOT(mousemove()));

}

Control::~Control()
{
    delete ui;
}

void Control::onTimeout()
{
    if(!isHidden())
    {
        setVisible(false);
    }
    timer->stop();
}

void Control::moveTime(){
    qDebug()<<mouse_x<<" "<<mouse_y;
    int x,y;
    x=QCursor().pos().x();
    y=QCursor().pos().y();
    if(x!=mouse_x||y!=mouse_y){
        if(isHidden())
            setVisible(true);
        timer->start(5000);
    }
    mouse_x=x;
    mouse_y=y;
    move->start(100);
}
/*
void Control::mousemove()
{
    if(isHidden())
    {
        setVisible(true);
    }
    timer->start(1000);
}
*/
