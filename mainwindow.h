#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "framelessmainwindow.h"
#include <QWidget>
#include <QListWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QFileSystemWatcher>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include "qvideowidget_p.h"
#include "control.h"
#include <QScreen>
#include <QRect>
#include <QMenu>
#include "player.h"
#include "playlist.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public FramelessMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //媒体库相关
    void highlight(int lastindex, int currentindex); //播放高亮
    void showPlaylist();

    //拖拽
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void add_file(QString filename);

    //媒体信息
    QStringList getMetaData();

    //文件监控
    void on_fileChanged(const QString &path);

    //子窗口（悬浮窗口）中要调用的函数
    Player* sendplayer(); //player
    QAudioOutput* sendaudio();  //AudioOutput
    qint64 sendDuration();  //Duration
    QString sendslider();
    int sendposition();
    float sendvolume();     //volume
    int rate_currentIndex();    //播放速率
    int mode_currentIndex();    //播放模式
    QIcon sendmuteButton();     //静音图标
    QIcon sendplayORpauseButton();   //播放/暂停图标


    void playORpause(); //播放/暂停
    void previous();    //上一首
    void next();    //下一首
    void drawback();    //快退
    void fastforward();     //快进
    void playmode();     //播放模式
    void on_ratebox_Changed(int index);  //倍速



private slots:
    //无边框
    void initForm();
    void titleDblClick();
    void windowStateChange(bool max);
    //无边框按键
    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Close_clicked();

    void playbackstatechange();

    void on_playORpause_clicked();  //播放暂停按钮

    void on_previousButton_clicked();   //上一首按钮

    void on_nextButton_clicked();   //下一首按钮

    void on_drawbackButton_clicked();   //快退

    void on_fastforwardButton_clicked();    //快进

    void updateDurationInfo(qint64 currentInfo);

    void durationChanged(qint64 duration);

    void positionChanged(qint64 progress);

    void on_ratebox_currentIndexChanged(int index);  //倍速

    void about_to_finish();     //播放模式

    void on_slider_sliderMoved(int position);

    void on_muteButton_clicked();   //静音按钮

    void on_fullScreenButton_clicked(); //全屏按钮

    void on_volumeSlider_sliderMoved(int position);

    void setVolume(float position);

    void changeVolumeicon(int position);

    void keyPressEvent(QKeyEvent *event);

    void on_playlist_itemDoubleClicked(QListWidgetItem *item); //双击

    void on_playlist_customContextMenuRequested(const QPoint &pos); //右键显示媒体信息

    void closeEvent (QCloseEvent * e);    //退出

    void metadatachange();

    void paintEvent(QPaintEvent *event);    //有缩放事件时重绘

    void shrink();  //接收小屏信号

    void deleteitem(); //删除媒体文件

    void on_deleteallButton_clicked();  //清空列表

    void on_addButton_clicked();

    void metadataDialog();//媒体信息窗口

    void on_deleteButton_clicked();

private:

    Ui::MainWindow *ui;
    Player *player;
    QVideoWidget *VideoWidget;
    QAudioOutput *AudioOutput;

    Control *control=nullptr;

    QMediaPlayer *playerForMetadata;

    // 播放列表
    PlayList playlist;
    int CurrentIndex;
    QMenu *menu = NULL;
    QFileSystemWatcher *FileWatcher;

    qint64 duration;
};
#endif // MAINWINDOW_H

