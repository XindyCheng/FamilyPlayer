#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QObject>
#include <QFileDialog>
#include <QStandardPaths>

class PlayList : public QObject
{
    Q_OBJECT
public:
    explicit PlayList(QObject *parent = nullptr);
    ~PlayList();
    int init();                 //初始化
    QString get_error_message();//显示错误信息
    int length();               //返回列表的长度
    QString & operator[](int i);//重载运算符
    QString get_listItem(int);
    int get_index(QString);
    void write();               //把数据写入txt文件
    int add_file(QString filename);//往列表中添加文件
    void removeAt(int row);     //删除一项
    int next_file(int current_index, int playmode); //根据playmode返回下一个文件的index

signals:

private:
    QString errorMessage="";
    QStringList my_list;
    QStringList suffixList={"mp3","mp4","mkv","flac","avi","wmv"};
};

#endif // PLAYLIST_H
