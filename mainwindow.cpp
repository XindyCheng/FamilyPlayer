#include "mainwindow.h"
#include "ui_mainwindow.h"
# include <QDebug>
//#include <QFileInfo>
#include <QMediaMetaData>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    player = new QMediaPlayer;
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

    ui->label->hide();

    //setMouseTracking(true);
    //ui->centralwidget->setMouseTracking(true);

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

    connect(player,&QMediaPlayer::metaDataChanged,this,&MainWindow::metadatachange);


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

void MainWindow::metadatachange(){

     /*
     ui->playlist->setAutoFillBackground(true); 
     QPalette pal(palette());
     //pal.setBrush(ui->widget->backgroundRole(), QBrush(pic.scaled(pic.size(), Qt::IgnoreAspectRatio,
                           //Qt::SmoothTransformation)));
     pal.setBrush(ui->playlist->backgroundRole(), QBrush(pic));
     ui->playlist->setPalette(pal);
     //ui->videowidget->setBackgroundRole();
     */
    //ui->widget->show();
    qDebug()<<this->player->metaData().value(QMediaMetaData::ThumbnailImage);
     if(this->player->metaData().value(QMediaMetaData::ThumbnailImage).isValid()){
         //ui->widget->setVisible(true);
         ui->videowidget->setVisible(false);
         //ui->videowidget->hide();

         QImage pic = this->player->metaData().value(QMediaMetaData::ThumbnailImage).value<QImage>();

         //QLabel *label=new QLabel(this);
         //ui->label->setGeometry(500,150,300,280);
         //QImage dest=pic.scaled(pic.size()*0.65,Qt::KeepAspectRatio);
         QImage dest=pic.scaled(ui->videowidget->size(),Qt::KeepAspectRatio);
         ui->label->setPixmap(QPixmap::fromImage(dest));

         ui->label->show();

         //QPixmap pix = QPixmap::fromImage(pic.scaled(ui->widget->size(),Qt::KeepAspectRatio));
         //QPalette pal;
         //pal.setBrush(ui->widget->backgroundRole(),QBrush(pix));
         //setPalette(pal);
         /*
         QPixmap pix = QPixmap::fromImage(pic.scaled(pic.size(),Qt::KeepAspectRatio));
         QPalette pal;
         pal.setBrush(backgroundRole(),QBrush(pix));
         setPalette(pal);
         */

     }
     else{
         qDebug()<<"??";
         ui->videowidget->setVisible(true);
         //ui->videowidget->show();
         ui->label->hide();
         //ui->widget->hide();
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
    //ui->playlist->setCurrentRow(currentindex);
    if(lastindex!=-1)
        ui->playlist->item(lastindex)->setBackground(QColor(255,255,255));
    ui->playlist->item(currentindex)->setBackground(QColor(205,232,255));
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
    int lastindex = this->CurrentIndex;
    this->CurrentIndex = this->Playlist.indexOf(filename);  //Playlist中是否已有该文件
    if(this->CurrentIndex == -1){   //没有在Playlist中
        this->CurrentIndex = this->Playlist.length();
        this->Playlist.append(filename);
        ui->playlist->addItem(QFileInfo(filename).fileName());
    }
    highlight(lastindex, this->CurrentIndex);
    player->setSource(QUrl::fromLocalFile(filename));
    player->play();

    ui->playORpause->setIcon(QPixmap(":/images/button-pause.PNG"));
    ui->playORpause->setToolTip("暂停");

    /*
    const int previousMediaCount = MediaPlaylist->mediaCount();
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
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
    */
}
int MainWindow::hasinMediaPlaylist(QString filename)    //应该不用了
{
    return this->Playlist.indexOf(filename);

    /*
    const int previousMediaCount = MediaPlaylist->mediaCount();
    for(int i=0;i<previousMediaCount;i++){
        if(MediaPlaylist->media(i)==url) return i;
    }
    return -1;
    */
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event){
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event){
    const QMimeData *qm = event->mimeData();
    QString filename = qm->urls()[0].toLocalFile();
    qDebug()<<filename;

    if(filename == "") return;
    int lastindex = this->CurrentIndex;
    this->CurrentIndex = this->Playlist.indexOf(filename);  //Playlist中是否已有该文件
    if(this->CurrentIndex == -1){   //没有在Playlist中
        this->CurrentIndex = this->Playlist.length();
        this->Playlist.append(filename);
        ui->playlist->addItem(QFileInfo(filename).fileName());
    }
    highlight(lastindex, this->CurrentIndex);
    player->setSource(QUrl::fromLocalFile(filename));
    player->play();

    ui->playORpause->setIcon(QPixmap(":/images/button-pause.PNG"));
    ui->playORpause->setToolTip("暂停");

}

//双击播放列表中某一项
void MainWindow::on_playlist_itemDoubleClicked(QListWidgetItem *item)
{
    int lastindex = this->CurrentIndex;
    this->CurrentIndex=item->listWidget()->currentRow();
    highlight(lastindex, this->CurrentIndex);
    player->setSource(QUrl::fromLocalFile(this->Playlist[this->CurrentIndex]));
    player->play();
    ui->playORpause->setIcon(QPixmap(":/images/button-pause.PNG"));
    ui->playORpause->setToolTip("暂停");
}

//控件
//播放/暂停键
void MainWindow::on_playORpause_clicked()
{
    if(player->playbackState()==player->PlayingState){
        player->pause();
        ui->playORpause->setIcon(QPixmap(":/images/button-play.PNG"));
        ui->playORpause->setToolTip("播放");
    }else{
        if(player->playbackState()==player->PausedState){
            player->play();
            ui->playORpause->setIcon(QPixmap(":/images/button-pause.PNG"));
            ui->playORpause->setToolTip("暂停");
        }
    }
}

//上一首
void MainWindow::on_previousButton_clicked()
{
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
void MainWindow::on_nextButton_clicked()
{
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
    if(ui->slider->value()-3000<0)
        on_slider_sliderMoved(0);
    else on_slider_sliderMoved(ui->slider->value()-3000);
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
    if(maxnum <= ui->slider->value()+3000)
        on_slider_sliderMoved(maxnum);
    else on_slider_sliderMoved(ui->slider->value()+3000);
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
    ui->videowidget->setFullScreen(!ui->videowidget->isFullScreen());
    //ui->videowidget->setMouseTracking(true);
    control = new Control(ui->videowidget);
    QList<QScreen *> list_screen =  QGuiApplication::screens();  //多显示器
    QRect rect = list_screen.at(0)->geometry();
    control->setGeometry(0,rect.height()-90,rect.width(),90);
    control->show();
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

    qDebug()<<"!!!!";
    int combox_index = ui->playmode->currentIndex();
    int lastindex;
    switch(combox_index){
    case 0:{    //单曲播放
        qDebug()<<"000";
        break;
    }

    case 1:{    //顺序播放
        qDebug()<<"111";
        lastindex = this->CurrentIndex;
        this->CurrentIndex = (this->CurrentIndex+1)%(this->Playlist.length());
        player->setSource(QUrl::fromLocalFile(this->Playlist[this->CurrentIndex]));
        highlight(lastindex, this->CurrentIndex);
        player->play();
        break;
    }

    case 2:{    //随机播放
        qDebug()<<"222";
        lastindex = this->CurrentIndex;
        srand(QTime(0,0,0).secsTo(QTime::currentTime()));
        this->CurrentIndex = rand()%(this->Playlist.length());
        if(this->CurrentIndex==lastindex&&this->Playlist.length()!=1)
            this->CurrentIndex=(this->CurrentIndex-1)%this->Playlist.length();
        player->setSource(QUrl::fromLocalFile(this->Playlist[this->CurrentIndex]));
        highlight(lastindex, this->CurrentIndex); player->play();
        break;
    }

    case 3:{    //单曲循环
        qDebug()<<"333";
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
