#include "player.h"

Player::Player(QObject *parent)
    : QObject{parent}
{
    ;
}

Player::~Player()
{
    ;
}

/*连接mplayer和player之间的信号和槽*/
void Player::init()
{
    //视频时长信号变化
    connect(mplayer, &QMediaPlayer::durationChanged, this, &Player::get_durationChanged);
    //播放进度信号变化
    connect(mplayer, &QMediaPlayer::positionChanged, this, &Player::get_positionChanged);
    //播完时判断当前播放模式
    connect(mplayer, &QMediaPlayer::mediaStatusChanged, this, &Player::get_mediaStatusChanged);
    //player状态切换信号（换播放/暂停图标）
    connect(mplayer, &QMediaPlayer::playbackStateChanged, this, &Player::get_playbackStateChanged);
    //媒体数据切换
    connect(mplayer, &QMediaPlayer::metaDataChanged, this, &Player::get_metaDataChanged);
}

bool Player::init_list()
{
    mlist = new PlayList();
    return mlist->init();
}

Player::Player(QAudioOutput *audio_output, QObject *video_outPut)
{
    mplayer = new QMediaPlayer;
    mplayer->setVideoOutput(video_outPut);
    mplayer->setAudioOutput(audio_output);
    currentIndex = -1;
    init();
}

int Player::list_length()
{
    return mlist->length();
}

QString Player::get_listItem(int index)
{
    return mlist->get_listItem(index);
}

int Player::get_listIndex(QString filename)
{
    return mlist->get_index(filename);
}

QString Player::get_list_error()
{
    return mlist->get_error_message();
}

bool Player::isPlaying()
{
    if(mplayer->playbackState()==mplayer->PlayingState){
        return 1;
    }
    else{
        return 0;
    }
}

bool Player::isPaused()
{
    if(mplayer->playbackState()==mplayer->PausedState){
        return 1;
    }
    else{
        return 0;
    }
}

bool Player::isEnd()
{
    if(mplayer->mediaStatus()==mplayer->EndOfMedia){
        return 1;
    }
    else{
        return 0;
    }
}

void Player::write_memory()
{
    mlist->write();
}

int Player::add_file(QString fname)
{
    return mlist->add_file(fname);
}

void Player::remove_file(int index)
{
    return mlist->removeAt(index);
}

int Player::next(int playmode)
{
    if(playmode == 0){      //单曲播放
        mplayer->pause();
        return -1;
    }

    int next_index = mlist->next_file(currentIndex,playmode);
    mplayer->setSource(QUrl::fromLocalFile(mlist->get_listItem(next_index)));
    currentIndex = next_index;

    mplayer->play();

    return currentIndex;

}

int Player::previous()
{
    //当前播放的不是第一首
    if(currentIndex>0)
    {
        currentIndex -= 1;
        mplayer->setSource(QUrl::fromLocalFile(mlist->get_listItem(currentIndex)));
    }
    else
    {
        currentIndex = mlist->length() - 1;//跳到列表最后一首
        mplayer->setSource(QUrl::fromLocalFile(mlist->get_listItem(currentIndex)));
    }

    mplayer->play();

    return currentIndex;
}

void Player::setCurrentIndex(int index)
{
    currentIndex = index;
}

void Player::playORpause()
{
    if(mplayer->playbackState()==mplayer->PlayingState){
        mplayer->pause();
    }else{
        if(mplayer->playbackState()==mplayer->PausedState){
            mplayer->play();
        }
    }
}

void Player::play(int index)
{
    mplayer->setSource(QUrl::fromLocalFile(mlist->get_listItem(index)));
    mplayer->play();
    currentIndex = index;
}

void Player::stop()
{
    mplayer->stop();
}

qint64 Player::get_duration()
{
    return mplayer->duration();
}

void Player::rateChange(int index)
{
    switch(index)
    {
        case 0:
            mplayer->setPlaybackRate(0.5);
            break;
        case 1:
            mplayer->setPlaybackRate(1.0);
            break;
        case 2:
            mplayer->setPlaybackRate(1.5);
            break;
        case 3:
            mplayer->setPlaybackRate(2.0);
            break;
    }
}

bool Player::metaData_isEmpty()
{
    return mplayer->metaData().isEmpty();
}

bool Player::thumbnailImage_isValid()
{
    return mplayer->metaData().value(QMediaMetaData::ThumbnailImage).isValid();
}

QImage Player::get_thumbnailImage()
{
    return mplayer->metaData().value(QMediaMetaData::ThumbnailImage).value<QImage>();
}

void Player::setPosition(int position)
{
    mplayer->setPosition(position);
}

//slots
void Player::get_durationChanged(qint64 duration)
{
    emit durationChanged(duration);
}
void Player::get_positionChanged(qint64 position)
{
    emit positionChanged(position);
}
void Player::get_metaDataChanged()
{
    emit metaDataChanged();
}
void Player::get_mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    emit mediaStatusChanged(status);
}
void Player::get_playbackStateChanged(QMediaPlayer::PlaybackState newState)
{
    emit playbackStateChanged(newState);
}
