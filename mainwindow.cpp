#pragma execution_character_set("utf-8")

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include <QDebug>
#include <QMediaMetaData>
#include <QMovie>

MainWindow::MainWindow(QWidget *parent)
    : FramelessMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->initForm();
    playerForMetadata = new QMediaPlayer;

    AudioOutput = new QAudioOutput;
    player = new Player(AudioOutput,ui->videowidget);
    //播放列表
    this->FileWatcher=new QFileSystemWatcher();
    connect(FileWatcher, &QFileSystemWatcher::fileChanged, this, &MainWindow::on_fileChanged);
    CurrentIndex = -1;
    showPlaylist();    //载入之前的播放列表
    this->setAcceptDrops(true);
    ui->playlist->setAcceptDrops(true);    //接受拖拽事件


    //音量进度条设置
    setVolume(AudioOutput->volume());
    ui->volumeSlider->setRange(0, 100);
    ui->slider->setRange(0, player->get_duration());

    //设置默认播放速率
    ui->ratebox->setCurrentIndex(1);

    ui->videowidget->hide();
    ui->labelwidget->show();
    ui->label->setPixmap(QPixmap(":/images/player.png"));

    //视频时长信号变化
    connect(player, &Player::durationChanged, this, &MainWindow::durationChanged);
    //播放进度信号变化
    connect(player, &Player::positionChanged, this, &MainWindow::positionChanged);
    //播完时判断当前播放模式
    connect(player, &Player::mediaStatusChanged, this, &MainWindow::about_to_finish);
    //player状态切换信号（换播放/暂停图标）
    connect(player, &Player::playbackStateChanged, this, &MainWindow::playbackstatechange);
    //媒体数据切换
    connect(player, &Player::metaDataChanged, this, &MainWindow::metadatachange);

    //音量变化信号
    connect(AudioOutput, &QAudioOutput::volumeChanged, this, &MainWindow::setVolume);

    //videowidget
    connect(ui->videowidget, &QVideoWidget_p::videomouseClick, this, &MainWindow::playORpause);
    connect(ui->videowidget, &QVideoWidget_p::videosetFullScreen, this, &MainWindow::on_fullScreenButton_clicked);
    connect(ui->videowidget, &QVideoWidget_p::videoclearFocus, this, &MainWindow::metadatachange);

    //labelwidget
    connect(ui->labelwidget, &QWidget_p::labelmouseClick, this, &MainWindow::playORpause);
    connect(ui->labelwidget, &QWidget_p::labelsetFullScreen, this, &MainWindow::on_fullScreenButton_clicked);
    connect(ui->labelwidget, &QWidget_p::labelclearFocus, this, &MainWindow::metadatachange);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initForm()
{
    //设置标题栏控件
    ui->labTitle->setText("FamilyPlayer");
    this->setWindowTitle(ui->labTitle->text());
    this->setTitleBar(ui->labTitle);

    //关联信号
    connect(this, SIGNAL(titleDblClick()), this, SLOT(titleDblClick()));
    connect(this, SIGNAL(windowStateChange(bool)), this, SLOT(windowStateChange(bool)));

}

void MainWindow::titleDblClick()
{
    on_btnMenu_Max_clicked();
}

void MainWindow::windowStateChange(bool max)
{
    ui->btnMenu_Max->setIcon(max ? QPixmap(":/images/arrows-shrink.PNG"): QPixmap(":/images/arrows-expand.PNG"));
    ui->btnMenu_Max->setToolTip(max ? "还原" : "最大");
}

void MainWindow::on_btnMenu_Min_clicked()
{
    this->showMinimized();
}

void MainWindow::on_btnMenu_Max_clicked()
{
    if (this->isMaximized()) {
        this->showNormal();
        ui->btnMenu_Max->setIcon(QPixmap(":/images/arrows-expand.PNG"));
        ui->btnMenu_Max->setToolTip("最大化");
    } else {
        this->showMaximized();
        ui->btnMenu_Max->setIcon(QPixmap(":/images/arrows-shrink.PNG"));
        ui->btnMenu_Max->setToolTip("还原");
    }
}

void MainWindow::on_btnMenu_Close_clicked()
{
    this->close();
}

//退出
void MainWindow::closeEvent (QCloseEvent * e){
    if(QMessageBox::question(this,tr("退出"),tr("您确定要退出吗？"),
                             QMessageBox::Yes, QMessageBox::No)== QMessageBox::Yes){
        player->write_memory();
        e->accept();//不会将事件传递给组件的父组件
    }
    else  e->ignore();

}

//播放状态切换
void MainWindow::playbackstatechange(){
    if(player->isPlaying()){
        ui->playORpause->setIcon(QPixmap(":/images/button-pause.PNG"));
        ui->playORpause->setToolTip("暂停");
    }
    else{
        ui->playORpause->setIcon(QPixmap(":/images/button-play.PNG"));
        ui->playORpause->setToolTip("播放");
    }
}

//媒体数据切换，封面重绘
void MainWindow::metadatachange(){
    if(this->player->metaData_isEmpty())
    {
        return;
    }
    //有封面
    if(this->player->thumbnailImage_isValid()){

        ui->labelwidget->show();
        QImage pic = this->player->get_thumbnailImage();
        QImage dest=pic.scaled(ui->labelwidget->size(),Qt::KeepAspectRatio);
        ui->label->setPixmap(QPixmap::fromImage(dest));

         if(ui->labelwidget->isFullScreen()||ui->videowidget->isFullScreen()){
             if(ui->videowidget->isFullScreen()){
                 ui->videowidget->setFullScreen(false);
                 ui->labelwidget->setWindowFlags(Qt::Window);
                 ui->labelwidget->showFullScreen();
             }
              if(control){
                      if(!control->isHidden()){
                          control->setVisible(false);
                          control->setFocus();
                      }
                  control->setVisible(true);
                  control->timer->start(5000);
              }
          }
         if(ui->videowidget->isVisible())
             ui->videowidget->setVisible(false);
          ui->labelwidget->setVisible(true);
    }
    //无封面
     else{
         if(ui->labelwidget->isFullScreen()||ui->videowidget->isFullScreen()){
             if(ui->labelwidget->isFullScreen()){
                 ui->labelwidget->setWindowFlags(Qt::SubWindow);
                 ui->labelwidget->showNormal();
                 ui->videowidget->setFullScreen(true);
             }

             if(control){
                 if(!control->isHidden()){
                     control->setVisible(false);
                     control->setFocus();
                 }
                 control->setVisible(true);
                 control->timer->start(5000);
             }
         }
         if(ui->labelwidget->isVisible())
             ui->labelwidget->setVisible(false);
         ui->videowidget->setVisible(true);
     }
}

//媒体库
//读取文件
void MainWindow::showPlaylist(){
    if(!player->init_list()){
        QMessageBox::warning(this, "提示", player->get_list_error());
    }
    int i = 0;
    while(i < player->list_length()){
        ui->playlist->addItem(QFileInfo(player->get_listItem(i)).fileName());
        this->FileWatcher->addPath(player->get_listItem(i));
        i++;
    }
}

//高亮
void MainWindow::highlight(int lastindex, int currentindex){
    if(lastindex!=-1)
        ui->playlist->item(lastindex)->setBackground(QColor(85,85,127));
    ui->playlist->item(currentindex)->setBackground(QColor(216,191,216));
    ui->playlist->setCurrentRow(currentindex);
}

void MainWindow::add_file(QString filename)
{
    if(filename == "") return;
    if(player->add_file(filename)==1){
        ui->playlist->addItem(QFileInfo(filename).fileName());
        this->FileWatcher->addPath(filename);
    }
    else{
        static bool isshow_flag = false;
        if(player->add_file(filename)==0){
            if (!isshow_flag) {
                    isshow_flag = true;
                    QMessageBox box;
                    box.setText("该音频/视频已在播放列表中！");//设置显示文本
                    box.setWindowTitle("添加音频/视频");//设置弹窗标题
                    box.exec();
                    isshow_flag = false;
            }
        }else{
            if (!isshow_flag) {
                    isshow_flag = true;
                    QMessageBox box;
                    box.setText("该资源不是可播放资源！");//设置显示文本
                    box.setWindowTitle("添加音频/视频");//设置弹窗标题
                    box.exec();
                    isshow_flag = false;
            }
        }
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event){
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event){
    const QMimeData *qm = event->mimeData();
    QString filename = qm->urls()[0].toLocalFile();
    qDebug()<<filename;
    add_file(filename);


}
//文件移动
void MainWindow::on_fileChanged(const QString &path)
{
    QString hint="文件"+path+"不存在或已被移动";
    QMessageBox::warning(this, "提示", hint);
    this->FileWatcher->removePath(path);
    int i = player->get_listIndex(path);
    QListWidgetItem *item=ui->playlist->takeItem(i);
    delete item;
    player->remove_file(i);;
}

//双击播放列表中某一项
void MainWindow::on_playlist_itemDoubleClicked(QListWidgetItem *item)
{
    int lastindex = this->CurrentIndex;
    this->CurrentIndex=item->listWidget()->currentRow();
    highlight(lastindex, this->CurrentIndex);
    player->play(CurrentIndex);

}
//删除选中音视频
void MainWindow::deleteitem()
{
    if(player->list_length()==0) return;
    int row=ui->playlist->currentRow();
    QString question = "您确定要将"+this->player->get_listItem(row)+"移出播放列表吗？";
    if(QMessageBox::question(this,tr("删除"), question,
                             QMessageBox::Yes, QMessageBox::No)== QMessageBox::Yes)
    {
        QListWidgetItem *item=ui->playlist->takeItem(row);
        delete item;
        this->FileWatcher->removePath(player->get_listItem(row));
        if(this->CurrentIndex==row){
            player->stop();
            ui->videowidget->hide();
            ui->labelwidget->show();
            ui->label->setPixmap(QPixmap(":/images/player.png"));
        }
        player->remove_file(row);
    }
    else{
        return;
    }

}
//清空列表
void MainWindow::on_deleteallButton_clicked()
{
    if(QMessageBox::question(this,tr("删除"),tr("您确定要清空播放列表吗？"),
                             QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {
        int len=player->list_length();
        for(int i=0;i<len;i++){
            this->FileWatcher->removePath(player->get_listItem(0));
            player->remove_file(0);
        }
        ui->playlist->clear();

        player->stop();
        ui->videowidget->hide();
        ui->labelwidget->show();
        ui->label->setPixmap(QPixmap(":/images/player.png"));
    }
    else{
        return;
    }
}
//右键单击媒体库列表文件，显示媒体信息
void MainWindow::on_playlist_customContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem* curItem = ui->playlist->itemAt(pos);
    if(curItem == NULL) return;

    if(menu)//保证同时只存在一个menu，及时释放内存
    {
           delete menu;
           menu = NULL;
     }
    menu = new QMenu(this);

    QAction *Metadata = new QAction("查看媒体信息",this);
    QAction *Delete = new QAction("删除",this);//删除选中音视频
    menu->addAction(Metadata);
    menu->addAction(Delete);

    connect(Metadata,&QAction::triggered,this,&MainWindow::metadataDialog);
    connect(Delete,&QAction::triggered,this,&MainWindow::deleteitem);

    menu->exec(QCursor::pos());
}

QStringList MainWindow::getMetaData(){
    int i=ui->playlist->currentRow();
    playerForMetadata->setSource(QUrl::fromLocalFile(player->get_listItem(i)));
    while(playerForMetadata->mediaStatus()==QMediaPlayer::LoadingMedia){
        QEventLoop loop;
        QTimer::singleShot(50,&loop,SLOT(quit()));
        loop.exec();
    }

    QString Filename = QFileInfo(player->get_listItem(i)).fileName();
    QString VideoBitRate = playerForMetadata->metaData().stringValue(QMediaMetaData::VideoBitRate);
    QString VideoFrameRate = playerForMetadata->metaData().stringValue(QMediaMetaData::VideoFrameRate);
    QString VideoCodec = playerForMetadata->metaData().stringValue(QMediaMetaData::VideoCodec);
    QString AudioBitRate = playerForMetadata->metaData().stringValue(QMediaMetaData::AudioBitRate);
    QString AudioCodec = playerForMetadata->metaData().stringValue(QMediaMetaData::AudioCodec);
    QString TrackNumber = playerForMetadata->metaData().stringValue(QMediaMetaData::TrackNumber);
    QString AlbumTitle = playerForMetadata->metaData().stringValue(QMediaMetaData::AlbumTitle);
    QString Author = playerForMetadata->metaData().stringValue(QMediaMetaData::Author);

    QStringList MetaData;
    MetaData.append(Filename);
    MetaData.append(VideoBitRate);
    MetaData.append(VideoFrameRate);
    MetaData.append(VideoCodec);
    MetaData.append(AudioBitRate);
    MetaData.append(AudioCodec);
    MetaData.append(TrackNumber);
    MetaData.append(AlbumTitle);
    MetaData.append(Author);
    return MetaData;
}

void MainWindow::metadataDialog()
{
    Dialog dialog;
    dialog.setList(this->getMetaData());
    dialog.exec();
}

//控件
//播放/暂停键
void MainWindow::on_playORpause_clicked()
{
    this->playORpause();
}

//上一首
void MainWindow::on_previousButton_clicked()
{
    this->previous();
}

//下一首
void MainWindow::on_nextButton_clicked()
{
   this->next();
}

//实时显示已播放时长
void MainWindow::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
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
void MainWindow::durationChanged(qint64 p_duration)
{
    duration = p_duration / 1000;
    ui->slider->setMaximum(p_duration);
}

//进度条实时显示视频进度
void MainWindow::positionChanged(qint64 progress)
{
    if (!ui->slider->isSliderDown())
        ui->slider->setValue(progress);

    updateDurationInfo(progress / 1000);
}

//拖动视频进度条
void MainWindow::on_slider_sliderMoved(int position)
{
    player->setPosition(position);
}

//进度快退按钮
void MainWindow::on_drawbackButton_clicked()
{
    this->drawback();
}

//进度快进按钮
void MainWindow::on_fastforwardButton_clicked()
{
    this->fastforward();
}

//静音切换
void MainWindow::on_muteButton_clicked()
{
    AudioOutput->setMuted(!AudioOutput->isMuted());
    changeVolumeicon(ui->volumeSlider->sliderPosition());

}

//全屏显示
void MainWindow::on_fullScreenButton_clicked()
{
    if(this->player->thumbnailImage_isValid()){
        ui->labelwidget->setWindowFlags(Qt::Window);
        ui->labelwidget->showFullScreen();
    }
    else
        ui->videowidget->setFullScreen(!ui->videowidget->isFullScreen());
    control = new Control(this);
    QList<QScreen *> list_screen =  QGuiApplication::screens();  //多显示器
    QRect rect = list_screen.at(0)->geometry();
    control->setGeometry(0,rect.height()-95,rect.width(),95);
    control->initslider();
    control->show();

    connect(control,&Control::shrink,this,&MainWindow::shrink);
    connect(ui->videowidget, &QVideoWidget_p::videoshrink, control, &Control::shrink_p);
    connect(ui->labelwidget, &QWidget_p::labelshrink, control, &Control::shrink_p);

}

//拖动音量进度条
void MainWindow::on_volumeSlider_sliderMoved(int position)
{
    qreal linearVolume =  QAudio::convertVolume(position / qreal(100),
                                                    QAudio::LogarithmicVolumeScale,
                                                    QAudio::LinearVolumeScale);
    AudioOutput->setVolume(linearVolume);
}

//音量改变后的设置
void MainWindow::setVolume(float volume)
{
    qreal logarithmicVolume = QAudio::convertVolume(volume,
                                                    QAudio::LinearVolumeScale,
                                                    QAudio::LogarithmicVolumeScale);
    int position = qRound(logarithmicVolume * 100);
    ui->volumeSlider->setValue(position);
    changeVolumeicon(position);
    ui->volumeNum->setText(QString::number(position+1,10));
}

//改变音量图标
void MainWindow:: changeVolumeicon(int position)
{
    if(AudioOutput->isMuted())
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

//快捷键设置
void MainWindow:: keyPressEvent(QKeyEvent *event)
{
    setFocus();
    if(event->modifiers()&Qt::ControlModifier)
    {
        if(event->key()==Qt::Key_Up)
        {
            on_volumeSlider_sliderMoved(ui->volumeSlider->value()+5);
        }
        else if(event->key()==Qt::Key_Down)
        {
            on_volumeSlider_sliderMoved(ui->volumeSlider->value()-5);
        }

    }
}

//播放模式
void MainWindow::about_to_finish()
{
    if(!player->isEnd()){
        return;
    }
    int next_index = player->next(ui->playmode->currentIndex());
    if(next_index != -1){
        highlight(this->CurrentIndex, next_index);
        this->CurrentIndex = next_index;
    }

}

//倍速播放
void MainWindow::on_ratebox_currentIndexChanged(int index)
{
    player->rateChange(index);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    ui->label->resize(ui->labelwidget->size());
}
void MainWindow::on_addButton_clicked()
{
    QFileDialog fileDialog(this);
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).value(0, QDir::homePath()));
    QString filename = fileDialog.getOpenFileName();
    qDebug()<<filename;
    add_file(filename);

}

void MainWindow::on_deleteButton_clicked()
{
    deleteitem();
}

//悬浮窗中要调用的函数
//传递重要参数到control中
Player* MainWindow::sendplayer()
{
    return player;
}

QAudioOutput* MainWindow::sendaudio()
{
    return AudioOutput;
}


float MainWindow::sendvolume()
{
    return AudioOutput->volume();
}

qint64 MainWindow::sendDuration()
{
    return duration;
}
QString MainWindow::sendslider()
{
    return ui->labelDuration->text();
}
int MainWindow::sendposition()
{
    return ui->slider->sliderPosition();
}
int MainWindow::rate_currentIndex()
{
    return ui->ratebox->currentIndex();
}

int MainWindow::mode_currentIndex()
{
    return ui->playmode->currentIndex();
}

QIcon MainWindow::sendmuteButton()
{
    return ui->muteButton->icon();
}

QIcon MainWindow::sendplayORpauseButton()
{
    return ui->playORpause->icon();
}

//缩屏
void MainWindow::shrink()
{

        ui->ratebox->setCurrentIndex(control->rate_currentIndex());
        ui->playmode->setCurrentIndex(control->mode_currentIndex());
        ui->muteButton->setIcon(control->sendmuteButton());
        if(ui->videowidget->isFullScreen()){
            //缩小屏幕时把全屏时的悬浮框control的播放速率和播放模式的值传到主屏幕中
        ui->videowidget->setFullScreen(false);
        ui->videowidget->showNormal();

    }
    if(ui->labelwidget->isFullScreen())
    {
        ui->labelwidget->setWindowFlags(Qt::SubWindow);
        ui->labelwidget->showNormal();
    }
    setFocus();
}
//播放暂停
void MainWindow::playORpause(){
    player->playORpause();
}

//上一首
void MainWindow::previous(){
    int next_index = player->previous();
    highlight(this->CurrentIndex, next_index);
    this->CurrentIndex = next_index;
}

//下一首
void MainWindow::next(){   
    int next_index = player->next(1);
    highlight(this->CurrentIndex, next_index);
    this->CurrentIndex = next_index;
}

//快退
void MainWindow::drawback()
{
    QString str=ui->labelDuration->text();
    QStringList list=str.split("/");
    if(ui->slider->value()-2000<0)
        on_slider_sliderMoved(0);
    else on_slider_sliderMoved(ui->slider->value()-2000);
}
//快进
void MainWindow::fastforward()
{
    QString str=ui->labelDuration->text();
    QStringList list=str.split("/");
    QStringList maxsum=list.at(1).split(":");
    int maxnum=0;
    for(int i=maxsum.size()-1;i>=0;i--)
    {
        maxnum+=maxsum.at(i).toInt()*qPow(60,(maxsum.size()-i-1))*1000;
    }
    if(maxnum <= ui->slider->value()+2000)
        on_slider_sliderMoved(maxnum);
    else on_slider_sliderMoved(ui->slider->value()+2000);
}
//倍速播放
void MainWindow::on_ratebox_Changed(int index)
{
    return on_ratebox_currentIndexChanged(index);
}
//播放模式
void MainWindow::playmode()
{
    if(ui->labelwidget->isFullScreen()||ui->videowidget->isFullScreen())
        ui->playmode->setCurrentIndex(control->mode_currentIndex());
    return about_to_finish();
}



