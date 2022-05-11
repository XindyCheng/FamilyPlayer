#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QtMultimediaWidgets/qvideowidget.h"
#include <QMainWindow>
#include <QWidget>
#include <QListWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QKeyEvent>
#include "qmediaplaylist.h"
#include "qvideowidget_p.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int hasinMediaPlaylist(QString filename);
    void highlight(int lastindex, int currentindex);
    void readPlaylist();
    void writePlaylist();

private slots:
    void on_pushButton_clicked();   //添加播放文件

    void on_playORpause_clicked();  //播放暂停按钮

    void on_previousButton_clicked();   //上一首按钮

    void on_nextButton_clicked();   //下一首按钮

    void updateDurationInfo(qint64 currentInfo);

    void durationChanged(qint64 duration);

    void positionChanged(qint64 progress);

    void on_slider_sliderMoved(int position);

    void on_muteButton_clicked();   //静音按钮

    void on_fullScreenButton_clicked(); //全屏按钮

    void on_volumeSlider_sliderMoved(int position);

    void setVolume(float position);

    void changeVolumeicon(int position);

    void keyPressEvent(QKeyEvent *event);

    void on_playlist_itemDoubleClicked(QListWidgetItem *item);

    void closeEvent (QCloseEvent * e);    //退出

private:
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QVideoWidget *VideoWidget;
    QAudioOutput *AudioOutput;

    // 播放列表
    QStringList Playlist;
    int CurrentIndex;

    qint64 duration;
};
#endif // MAINWINDOW_H
