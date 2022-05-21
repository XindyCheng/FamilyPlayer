#include "mainwindow.h"
#include "ui_mainwindow.h"
# include <QDebug>
//#include <QFileInfo>
#include <QMediaMetaData>
#include<QMovie>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    player = new QMediaPlayer;
    playerForMetadata = new QMediaPlayer;
    //播放窗口
    player->setVideoOutput(ui->videowidget);

    //默认声音
    AudioOutput = new QAudioOutput;
    player->setAudioOutput(AudioOutput);

    //音量进度条设置
    setVolume(AudioOutput->volume());
    ui->volumeSlider->setRange(0, 100);
    ui->slider->setRange(0, player->duration());

    //设置默认播放速率
    ui->ratebox->setCurrentIndex(1);

    //播放列表
    CurrentIndex = -1;
    readPlaylist();    //载入之前的播放列表
    this->setAcceptDrops(true);
    ui->playlist->setAcceptDrops(true);    //接受拖拽事件

    ui->labelwidget->hide();

    //菜单栏-添加
    connect(ui->actionopenfile,SIGNAL(triggered()),this,SLOT(actionOpenSlot()));

    //视频时长信号变化
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    //播放进度信号变化
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    //音量变化信号
    connect(AudioOutput, &QAudioOutput::volumeChanged, this, &MainWindow::setVolume);

    //播完时判断当前播放模式
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::about_to_finish);

    //player状态切换信号（换播放/暂停图标）
    connect(player, &QMediaPlayer::playbackStateChanged, this, &MainWindow::playbackstatechange);
    //媒体数据切换
    connect(player, &QMediaPlayer::metaDataChanged, this, &MainWindow::metadatachange);

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

//退出
void MainWindow::closeEvent (QCloseEvent * e){
    if(QMessageBox::question(this,tr("退出"),tr("您确定要退出吗？"),
                             QMessageBox::Yes, QMessageBox::No)== QMessageBox::Yes){
        writePlaylist();
        e->accept();//不会将事件传递给组件的父组件
    }
    else  e->ignore();

}

//播放状态切换
void MainWindow::playbackstatechange(){
    if(player->playbackState()==player->PlayingState){
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
    if(this->player->metaData().isEmpty())
    {
        return;
    }
    //有封面
    if(this->player->metaData().value(QMediaMetaData::ThumbnailImage).isValid()){

        ui->labelwidget->show();
        QImage pic = this->player->metaData().value(QMediaMetaData::ThumbnailImage).value<QImage>();
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
void MainWindow::readPlaylist(){
    QFile file("./playlist.txt");
    if(!file.exists()){
        qDebug()<<"playlist文件不存在，创建文件";
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        file.close();
    }
    else{
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString filename;
        while (!file.atEnd()){
            filename = file.readLine();
            filename= filename.trimmed();
            if(filename == "") break;
            qDebug()<<filename;
            if(QFile::exists(filename)){
                this->Playlist.append(filename);
                ui->playlist->addItem(QFileInfo(filename).fileName());
            }
            else{
                QString hint="文件"+filename+"不存在或已被移动";
                QMessageBox::warning(this, "提示", hint);
            }
        }
    }
}

//写入文件
void MainWindow::writePlaylist(){
    QFile file("./playlist.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QString res="";
    for(int i=0;i<this->Playlist.length();i++){
        res = this->Playlist[i]+'\n';
        file.write(res.toUtf8());
    }
    file.close();
}

//高亮
void MainWindow::highlight(int lastindex, int currentindex){
    if(lastindex!=-1)
        ui->playlist->item(lastindex)->setBackground(QColor(85,85,127));
    ui->playlist->item(currentindex)->setBackground(QColor(216,191,216));
    ui->playlist->setCurrentRow(currentindex);
}

//添加到媒体库
void MainWindow::actionOpenSlot()
{
    QFileDialog fileDialog(this);
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).value(0, QDir::homePath()));

    QString filename = fileDialog.getOpenFileName();
    qDebug()<<filename;
    if(filename == "") return;
    //int lastindex = this->CurrentIndex;

    int currentIndex = this->Playlist.indexOf(filename); //Playlist中是否已有该文件
    if(this->Playlist.length()==0)
    {
        this->CurrentIndex=this->Playlist.length();
        this->Playlist.append(filename);
        ui->playlist->addItem(QFileInfo(filename).fileName());
    }
    else if(currentIndex == -1){   //没有在Playlist中
        //this->CurrentIndex = this->Playlist.length();
        this->Playlist.append(filename);
        ui->playlist->addItem(QFileInfo(filename).fileName());
    }
    else
    {
        static bool isshow_flag = false;
            if (!isshow_flag) {
                isshow_flag = true;
                QMessageBox box;
                box.setText("该音频/视频已在播放列表中！");//设置显示文本
                box.setWindowTitle("添加音频/视频");//设置弹窗标题
                box.exec();
                isshow_flag = false;
        }
    }
    //highlight(lastindex, this->CurrentIndex);
    //player->setSource(QUrl::fromLocalFile(filename));
    //player->play();

}

void MainWindow::dragEnterEvent(QDragEnterEvent *event){
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event){
    const QMimeData *qm = event->mimeData();
    QString filename = qm->urls()[0].toLocalFile();
    qDebug()<<filename;

    if(filename == "") return;
    //int lastindex = this->CurrentIndex;
    int currentIndex = this->Playlist.indexOf(filename);  //Playlist中是否已有该文件
    if(this->Playlist.length()==0)
    {
        this->CurrentIndex=this->Playlist.length();
        this->Playlist.append(filename);
        ui->playlist->addItem(QFileInfo(filename).fileName());
    }
    else if(currentIndex == -1){   //没有在Playlist中
        //this->CurrentIndex = this->Playlist.length();
        this->Playlist.append(filename);
        ui->playlist->addItem(QFileInfo(filename).fileName());
    }
    else
    {
        static bool isshow_flag = false;
            if (!isshow_flag) {
                isshow_flag = true;
                QMessageBox box;
                box.setText("该音频/视频已在播放列表中！");//设置显示文本
                box.setWindowTitle("添加音频/视频");//设置弹窗标题
                box.exec();
                isshow_flag = false;
        }
    }
    /*
    highlight(lastindex, this->CurrentIndex);
    player->setSource(QUrl::fromLocalFile(filename));
    player->play();
*/
}

//双击播放列表中某一项
void MainWindow::on_playlist_itemDoubleClicked(QListWidgetItem *item)
{
    int lastindex = this->CurrentIndex;
    this->CurrentIndex=item->listWidget()->currentRow();
    highlight(lastindex, this->CurrentIndex);
    player->setSource(QUrl::fromLocalFile(this->Playlist[this->CurrentIndex]));
    player->play();

}
//删除选中音视频
void MainWindow::deleteitem()
{
    if(QMessageBox::question(this,tr("删除"),tr("您确定要删除吗？"),
                             QMessageBox::Yes, QMessageBox::No)== QMessageBox::Yes)
    {
        int row=ui->playlist->currentRow();
        QListWidgetItem*item=ui->playlist->takeItem(row);
        delete item;
        this->Playlist.removeAt(row);
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

    int i=ui->playlist->currentRow();
    //qDebug()<<i;
    //qDebug()<<this->Playlist[index];
    playerForMetadata->setSource(QUrl::fromLocalFile(this->Playlist[i]));
    //qDebug()<<playerForMetadata->mediaStatus();
    while(playerForMetadata->mediaStatus()==QMediaPlayer::LoadingMedia){
        QEventLoop loop;
        QTimer::singleShot(50,&loop,SLOT(quit()));
        loop.exec();
    }

    QString Filename = QFileInfo(this->Playlist[i]).fileName();
    QString VideoBitRate = playerForMetadata->metaData().stringValue(QMediaMetaData::VideoBitRate);
    QString VideoFrameRate = playerForMetadata->metaData().stringValue(QMediaMetaData::VideoFrameRate);
    QString VideoCodec = playerForMetadata->metaData().stringValue(QMediaMetaData::VideoCodec);
    QString AudioBitRate = playerForMetadata->metaData().stringValue(QMediaMetaData::AudioBitRate);
    QString AudioCodec = playerForMetadata->metaData().stringValue(QMediaMetaData::AudioCodec);
    QString TrackNumber = playerForMetadata->metaData().stringValue(QMediaMetaData::TrackNumber);
    QString AlbumTitle = playerForMetadata->metaData().stringValue(QMediaMetaData::AlbumTitle);
    QString Author = playerForMetadata->metaData().stringValue(QMediaMetaData::Author);

    menu = new QMenu(this);
    QAction *filename = new QAction("文件名： "+Filename, this);
    QAction *videoBitRate = new QAction("视频码率： "+VideoBitRate, this);
    QAction *videoFrameRate = new QAction("视频帧率： "+VideoFrameRate, this);
    QAction *videoCodec = new QAction("视频编码方式： "+VideoCodec, this);
    QAction *audioBitRate = new QAction("音频码率： "+AudioBitRate, this);
    QAction *audioCodec = new QAction("音频编码方式： "+AudioCodec, this);
    QAction *trackNumber = new QAction("音频声道数： "+TrackNumber, this);
    QAction *albumTitle = new QAction("专辑标题： "+AlbumTitle, this);
    QAction *author = new QAction("演唱者： "+Author, this);
    QAction *Delete = new QAction("删除",this);//删除选中音视频

    menu->addAction(filename);
    menu->addAction(videoBitRate);
    menu->addAction(videoFrameRate);
    menu->addAction(videoCodec);
    menu->addAction(audioBitRate);
    menu->addAction(audioCodec);
    menu->addAction(trackNumber);
    menu->addAction(albumTitle);
    menu->addAction(author);
    menu->addAction(Delete);

    connect(Delete,&QAction::triggered,this,&MainWindow::deleteitem);

    menu->exec(QCursor::pos());
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

//上一首
void MainWindow::on_previousButton_clicked()
{
    int playrow=this->Playlist.count();//播放列表总长度
    int state=0;//表示初始状态为播放还是暂停，初始值为0，表示处于暂停状态
    //如果当前处于播放状态，先暂停播放，state=1
    if(player->playbackState()==player->PlayingState)
    {
        player->pause();
        state=1;
    }
    //当前播放的不是第一首
    if(this->CurrentIndex>0)
    {
        int lastindex=this->CurrentIndex;
        this->CurrentIndex=this->CurrentIndex-1;
        highlight(lastindex,this->CurrentIndex);
        player->setSource(QUrl::fromLocalFile(this->Playlist[this->CurrentIndex]));
    }
    else
    {
        int lastindex=this->CurrentIndex;
        this->CurrentIndex=playrow-1;//跳到列表最后一首
        highlight(lastindex,this->CurrentIndex);
        player->setSource(QUrl::fromLocalFile(this->Playlist[this->CurrentIndex]));
    }
    if(state==0)player->pause();
    else if(state==1)player->play();

}

//下一首
void MainWindow::on_nextButton_clicked()
{
    int playrow=this->Playlist.count();
    int state=0;//表示初始状态为播放还是暂停，初始值为0，表示处于暂停状态
    //如果当前处于播放状态，先暂停播放，state=1
    if(player->playbackState()==player->PlayingState)
    {
        state=1;
        player->pause();
    }
    //当前播放的不是最后一首
    if(this->CurrentIndex<playrow-1)
    {
        int lastindex=this->CurrentIndex;
        this->CurrentIndex=this->CurrentIndex+1;
        highlight(lastindex,this->CurrentIndex);
        player->setSource(QUrl::fromLocalFile(this->Playlist[this->CurrentIndex]));
    }
    else
    {
        int lastindex=this->CurrentIndex;
        this->CurrentIndex=0;//跳到列表第一首
        highlight(lastindex,this->CurrentIndex);
        player->setSource(QUrl::fromLocalFile(this->Playlist[this->CurrentIndex]));
    }
    if(state==0)player->pause();
    else if(state==1)player->play();
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

    QString str=ui->labelDuration->text();
    QStringList list=str.split("/");
    if(ui->slider->value()-2000<0)
        on_slider_sliderMoved(0);
    else on_slider_sliderMoved(ui->slider->value()-2000);
}

//进度快进按钮
void MainWindow::on_fastforwardButton_clicked()
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

//静音切换
void MainWindow::on_muteButton_clicked()
{
    AudioOutput->setMuted(!AudioOutput->isMuted());
    changeVolumeicon(ui->volumeSlider->sliderPosition());

}

//全屏显示
void MainWindow::on_fullScreenButton_clicked()
{
    if(this->player->metaData().value(QMediaMetaData::ThumbnailImage).isValid()){
        ui->labelwidget->setWindowFlags(Qt::Window);
        ui->labelwidget->showFullScreen();
    }
    else
        ui->videowidget->setFullScreen(!ui->videowidget->isFullScreen());
    control = new Control(this);
    QList<QScreen *> list_screen =  QGuiApplication::screens();  //多显示器
    QRect rect = list_screen.at(0)->geometry();
    control->setGeometry(0,rect.height()-90,rect.width(),90);
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
    if(player->mediaStatus()!=player->EndOfMedia){
        return;
    }
    int combox_index = ui->playmode->currentIndex();
    int lastindex;
    switch(combox_index){
    case 0:{    //单曲播放
        break;
    }

    case 1:{    //顺序播放
        lastindex = this->CurrentIndex;
        this->CurrentIndex = (this->CurrentIndex+1)%(this->Playlist.length());
        player->setSource(QUrl::fromLocalFile(this->Playlist[this->CurrentIndex]));
        highlight(lastindex, this->CurrentIndex);
        player->play();
        break;
    }

    case 2:{    //随机播放
        lastindex = this->CurrentIndex;
        srand(QTime(0,0,0).secsTo(QTime::currentTime()));
        this->CurrentIndex = rand()%(this->Playlist.length());
        player->setSource(QUrl::fromLocalFile(this->Playlist[this->CurrentIndex]));
        highlight(lastindex, this->CurrentIndex); player->play();
        break;
    }

    case 3:{    //单曲循环
        player->setSource(QUrl::fromLocalFile(this->Playlist[this->CurrentIndex]));
        player->play();
        break;
    }
    }
}

//倍速播放
void MainWindow::on_ratebox_currentIndexChanged(int index)
{
    switch(index)
    {
        case 0:
            player->setPlaybackRate(0.5);
            break;
        case 1:
            player->setPlaybackRate(1.0);
            break;
        case 2:
            player->setPlaybackRate(1.5);
            break;
        case 3:
            player->setPlaybackRate(2.0);
            break;
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    ui->label->resize(ui->labelwidget->size());
}


//悬浮窗中要调用的函数
//传递重要参数到control中
QMediaPlayer* MainWindow::sendplayer()
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
    if(player->playbackState()==player->PlayingState){
        player->pause();
    }else{
        if(player->playbackState()==player->PausedState){
            player->play();
        }
    }
}
//上一首
void MainWindow::previous(){
    player->pause();
    int playrow=this->Playlist.count();//播放列表总长度
    //当前播放的不是第一首
    if(this->CurrentIndex>0)
    {
        int lastindex=this->CurrentIndex;
        this->CurrentIndex=this->CurrentIndex-1;
        highlight(lastindex,this->CurrentIndex);
        player->setSource(QUrl::fromLocalFile(this->Playlist[this->CurrentIndex]));
    }
    else
    {
        int lastindex=this->CurrentIndex;
        this->CurrentIndex=playrow-1;//跳到列表最后一首
        highlight(lastindex,this->CurrentIndex);
        player->setSource(QUrl::fromLocalFile(this->Playlist[this->CurrentIndex]));
    }
    player->play();
}
//下一首
void MainWindow::next(){
    int playrow=this->Playlist.count();

    player->pause();
    //当前播放的不是最后一首
    if(this->CurrentIndex<playrow-1)
    {
        int lastindex=this->CurrentIndex;
        this->CurrentIndex=this->CurrentIndex+1;
        highlight(lastindex,this->CurrentIndex);
        player->setSource(QUrl::fromLocalFile(this->Playlist[this->CurrentIndex]));
        player->play();
    }
    else
    {
        int lastindex=this->CurrentIndex;
        this->CurrentIndex=0;//跳到列表第一首
        highlight(lastindex,this->CurrentIndex);
        player->setSource(QUrl::fromLocalFile(this->Playlist[this->CurrentIndex]));
        player->play();
    }
}
//快退
void MainWindow::drawback()
{
    QString str=ui->labelDuration->text();
    QStringList list=str.split("/");
    if(ui->slider->value()-3000<0)
        on_slider_sliderMoved(0);
    else on_slider_sliderMoved(ui->slider->value()-3000);
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
    if(maxnum <= ui->slider->value()+3000)
        on_slider_sliderMoved(maxnum);
    else on_slider_sliderMoved(ui->slider->value()+3000);
}
//倍速播放
void MainWindow::on_ratebox_Changed(int index)
{
    return on_ratebox_currentIndexChanged(index);
}
//播放模式
void MainWindow::playmode()
{
    return about_to_finish();
}
void MainWindow::openslot()
{
    return actionOpenSlot();
}
