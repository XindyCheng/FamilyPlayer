#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QtMultimediaWidgets/qvideowidget.h"
#include <QMainWindow>
#include <QWidget>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include "qmediaplaylist.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int hasinMediaPlaylist(QUrl&);

private slots:
    void on_pushButton_clicked();

    void on_playORpause_clicked();

private:
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QVideoWidget *VideoWidget;
    QAudioOutput *AudioOutput;
    QMediaPlaylist *MediaPlaylist;
};
#endif // MAINWINDOW_H
