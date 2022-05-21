#include "control.h"
#include "qevent.h"
#include "ui_control.h"
#include "mainwindow.h"

Control::Control(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Control)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);

    // 把传过来的父窗口类的指针强制类型转换
    m_parent = static_cast<MainWindow*>(parent);
    //音量、进度条设置与主屏幕相同
    set_volume(m_parent->sendvolume());
    ui->volumeSlider->setRange(0, 100);
    //视频进度条、播放模式、播放速率与主屏幕相同
    ui->slider->setRange(0, m_parent->sendplayer()->duration());
    ui->playmode->setCurrentIndex(m_parent->mode_currentIndex());
    ui->ratebox->setCurrentIndex(m_parent->rate_currentIndex());
    //静音键、播放/暂停键与主屏幕相同
    ui->muteButton->setIcon(m_parent->sendmuteButton());
    ui->playORpause->setIcon(m_parent->sendplayORpauseButton());


    timer = new QTimer;
    move = new QTimer;

    mouse_x=QCursor().pos().x();
    mouse_y=QCursor().pos().y();

    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect(move, SIGNAL(timeout()), this, SLOT(moveTime()));


    move->start(100);

    //player状态切换信号（换播放/暂停图标）
    connect(m_parent->sendplayer(), &QMediaPlayer::playbackStateChanged, this, &Control::playbackstatechange);

    //音量变化信号
    connect(m_parent->sendaudio(), &QAudioOutput::volumeChanged, this, &Control::set_volume);

    //视频时长信号变化
    connect(m_parent->sendplayer(), &QMediaPlayer::durationChanged, this, &Control::durationChanged);
    //播放进度信号变化
    connect(m_parent->sendplayer(), &QMediaPlayer::positionChanged, this, &Control::positionChanged);

    //播完时判断当前播放模式
    connect(m_parent->sendplayer(), &QMediaPlayer::mediaStatusChanged, this, &Control::about_to_finish);
}

Control::~Control()
{
    delete ui;
}

void Control::moveTime(){
    int x,y;
    x=this->mapFromGlobal(QCursor().pos()).x();
    y=this->mapFromGlobal(QCursor().pos()).y();
    if(x!=mouse_x||y!=mouse_y){
        setVisible(true);
        timer->start(5000);
    }
    mouse_x=x;
    mouse_y=y;
}

void Control::onTimeout()
{
    if(!isHidden())
    {
        setVisible(false);
    }
    timer->stop();
}

//缩屏按键
void Control::on_fullScreenButton_clicked()
{
    emit shrink();
    move->stop();
    this->close();
}

void Control::shrink_p()
{
    on_fullScreenButton_clicked();
}

//播放速率参数
int Control::rate_currentIndex()
{
    return ui->ratebox->currentIndex();
}
//播放模式参数
int Control::mode_currentIndex()
{
    return ui->playmode->currentIndex();
}
//静音图标
QIcon Control::sendmuteButton()
{
    return ui->muteButton->icon();
}
//播放/暂停图标
QIcon Control::sendplayORpauseButton()
{
    return ui->playORpause->icon();
}


//播放状态图标切换
void Control::playbackstatechange(){
    if(m_parent->sendplayer()->playbackState()==m_parent->sendplayer()->PlayingState){
        ui->playORpause->setIcon(QPixmap(":/images/button-pause.PNG"));
        ui->playORpause->setToolTip("暂停");
    }
    else if(m_parent->sendplayer()->playbackState()==m_parent->sendplayer()->PausedState){
        ui->playORpause->setIcon(QPixmap(":/images/button-play.PNG"));
        ui->playORpause->setToolTip("播放");
    }
}

//播放/暂停键
void Control::on_playORpause_clicked()
{
    m_parent->playORpause();
}

//下一首
void Control::on_nextButton_clicked()
{
    m_parent->next();
}

//上一首
void Control::on_previousButton_clicked()
{
    m_parent->previous();
}

//进度快退
void Control::on_fastforwardButton_clicked()
{
    m_parent->fastforward();
}

//进度快进
void Control::on_drawbackButton_clicked()
{
    m_parent->drawback();
}

//播放模式
void Control::about_to_finish()
{
    m_parent->playmode();
}

//倍速播放
void Control::on_ratebox_currentIndexChanged(int index)
{
    m_parent->on_ratebox_Changed(index);
}

//静音
void Control::on_muteButton_clicked()
{
    //m_parent->mute();
    m_parent->sendaudio()->setMuted(!m_parent->sendaudio()->isMuted());
    change_Volumeicon(ui->volumeSlider->sliderPosition());
}

//拖动音量变化条
void Control::on_volumeSlider_sliderMoved(int position)
{
    qreal linearVolume =  QAudio::convertVolume(position / qreal(100),
                                                    QAudio::LogarithmicVolumeScale,
                                                    QAudio::LinearVolumeScale);
    m_parent->sendaudio()->setVolume(linearVolume);
}

//改变音量图标
void Control:: change_Volumeicon(int position)
{
    if(m_parent->sendaudio()->isMuted())
    {
        ui->muteButton->setIcon(QPixmap(":/images/volume-off.PNG"));
    }
    else if(position>50)
    {
        ui->muteButton->setIcon(QPixmap(":/images/volume-high.PNG"));
    }
    else
    {
        ui->muteButton->setIcon(QPixmap(":/images/volume-low.PNG"));
    }
}

//音量改变后的设置
void Control::set_volume(float volume)
{
    qreal logarithmicVolume = QAudio::convertVolume(volume,
                                                    QAudio::LinearVolumeScale,
                                                    QAudio::LogarithmicVolumeScale);
    int position = qRound(logarithmicVolume * 100);
    ui->volumeSlider->setValue(position);
    change_Volumeicon(position);
    ui->volumeNum->setText(QString::number(position+1,10));
}

void Control::initslider()
{
    ui->labelDuration->setText(m_parent->sendslider());
    ui->slider->setSliderPosition(m_parent->sendposition());
}
//实时显示已播放时长
void Control::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    duration=m_parent->sendDuration();
    if (currentInfo || duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
            currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((duration / 3600) % 60, (duration / 60) % 60,
            duration % 60, (duration * 1000) % 1000);
        QString format = "mm:ss";
        if (duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }    
    ui->labelDuration->setText(tStr);
}

//视频时长变化
void Control::durationChanged(qint64 p_duration)
{
    //duration=p_duration/1000;
    ui->slider->setMaximum(p_duration);
}

//进度条实时显示视频进度
void Control::positionChanged(qint64 progress)
{
    if (!ui->slider->isSliderDown())
        ui->slider->setValue(progress);

    updateDurationInfo(progress / 1000);
}
//拖动视频进度条
void Control::on_slider_sliderMoved(int position)
{
    m_parent->sendplayer()->setPosition(position);
}


//快捷键
void  Control::keyPressEvent(QKeyEvent *event)
{
    setFocus();
    if(event->modifiers()&Qt::ControlModifier)
    {
        //qDebug()<<event->key();
        if(event->key()==Qt::Key_Up)
        {
            on_volumeSlider_sliderMoved(ui->volumeSlider->value()+5);
        }
        else if(event->key()==Qt::Key_Down)
        {
            on_volumeSlider_sliderMoved(ui->volumeSlider->value()-5);
        }
        else if(event->key()==Qt::Key_I)
        {
            m_parent->openslot();
        }
    }
}


