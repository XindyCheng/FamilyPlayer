#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>
#include <QMimeData>
#include <QMediaMetaData>
#include <QWidget>
#include "playlist.h"


class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = nullptr);
    Player(QAudioOutput *audio_output, QObject *video_outPut);
    ~Player();
    bool isPlaying();
    bool isPaused();
    bool isEnd();
    void init();
    bool init_list();
    void write_memory();
    int list_length();
    QString get_listItem(int);
    int get_listIndex(QString);
    QString get_list_error();
    qint64 get_duration();
    int add_file(QString);
    void remove_file(int);
    int next(int playmode);
    int previous();
    void setCurrentIndex(int);
    void playORpause();
    void play(int index);
    void stop();
    void rateChange(int index);
    bool metaData_isEmpty();
    bool thumbnailImage_isValid();
    QImage get_thumbnailImage();
    void setPosition(int position);

public Q_SLOTS:
    void get_durationChanged(qint64 duration);
    void get_positionChanged(qint64 position);
    void get_metaDataChanged();
    void get_mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void get_playbackStateChanged(QMediaPlayer::PlaybackState newState);

Q_SIGNALS:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void metaDataChanged();
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void playbackStateChanged(QMediaPlayer::PlaybackState newState);

private:
    QMediaPlayer* mplayer;
    PlayList* mlist;
    int currentIndex;

};

#endif // PLAYER_H
