#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    player = new QMediaPlayer;
    //播放窗口
    VideoWidget = new QVideoWidget(ui->label);
    VideoWidget->resize(ui->label->size());
    player->setVideoOutput(VideoWidget);
    //默认声音
    AudioOutput = new QAudioOutput;
    player->setAudioOutput(AudioOutput);
    //播放列表
    MediaPlaylist = new QMediaPlaylist;

}

MainWindow::~MainWindow()
{
    delete ui;
}


//媒体库
//添加到媒体库
void MainWindow::on_pushButton_clicked()
{
    const int previousMediaCount = MediaPlaylist->mediaCount();

    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).value(0, QDir::homePath()));
    if (fileDialog.exec() == QDialog::Accepted){
        QList<QUrl> urls = fileDialog.selectedUrls();
        for (auto &url: urls){
            int index = hasinMediaPlaylist(url);
            if(index != -1){
                MediaPlaylist->setCurrentIndex(index);
            }else{
                MediaPlaylist->addMedia(url);
                MediaPlaylist->setCurrentIndex(previousMediaCount);
                //qDebug() <<QFileInfo(url.path()).fileName();
                ui->listWidget->addItem(QFileInfo(url.path()).fileName());
            }
        }

        player->setSource(MediaPlaylist->currentMedia());
        player->play();
    }
}
int MainWindow::hasinMediaPlaylist(QUrl& url)
{
    const int previousMediaCount = MediaPlaylist->mediaCount();
    for(int i=0;i<previousMediaCount;i++){
        if(MediaPlaylist->media(i)==url) return i;
    }
    return -1;
}

//控件
//播放/暂停键
void MainWindow::on_playORpause_clicked()
{
    if(player->playbackState()==player->PlayingState){
        player->pause();
    }else{
        if(player->playbackState()==player->PausedState){
            player->play();
        }
    }
}

