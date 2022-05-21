#ifndef CONTROL_H
#define CONTROL_H

#include <QDialog>
#include <QTimer>
#include <QAudioOutput>

class MainWindow;

namespace Ui {
class Control;
}

class Control : public QDialog
{
    Q_OBJECT

public:
    explicit Control(QWidget *parent = nullptr);
    ~Control();

    /*返回到mainwindow中的重要参数*/
    int rate_currentIndex();
    int mode_currentIndex();
    QIcon sendmuteButton();
    QIcon sendplayORpauseButton();
    //qint64 sendDuration();

    QTimer *timer;
    QTimer *move;

signals:
    void shrink();

public slots:
    //videowidget和labelwidget需要调用的函数
    void shrink_p();

private slots:
    void onTimeout();

    void moveTime();

    void on_fullScreenButton_clicked();

    void on_playORpause_clicked();

    void on_nextButton_clicked();

    void on_previousButton_clicked();

    void on_fastforwardButton_clicked();

    void on_drawbackButton_clicked();

    void about_to_finish();

    void on_ratebox_currentIndexChanged(int index);

    void playbackstatechange();

    void set_volume(float volume);

    void change_Volumeicon(int position);

    void on_muteButton_clicked();

    void on_volumeSlider_sliderMoved(int position);

    void updateDurationInfo(qint64 currentInfo);

    void durationChanged(qint64 p_duration);

    void positionChanged(qint64 progress);

    void on_slider_sliderMoved(int position);

    void  keyPressEvent(QKeyEvent *event);

private:
    Ui::Control *ui;

    MainWindow *m_parent;



    qint64 duration;

    int mouse_x;
    int mouse_y;
};

#endif // CONTROL_H
