#include "playlist.h"

PlayList::PlayList(QObject *parent)
    : QObject{parent}
{
    ;
}

PlayList::~PlayList()
{
    ;
}

int PlayList::init()
{
    //初始化playlist文件
    //QFile file("./playlist.txt");
    QString dir=QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/AppData/Local/FamilyPlayer";
    QDir Dir(dir);
    if(!Dir.exists()){
        qDebug()<<"FamilyPlayer文件夹不存在，创建文件夹";
        Dir.mkdir(dir);
    }
    QFile file(dir+"/playlist.txt");
    int mark = 1; //若有文件不存在了，则为0
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
                my_list.append(filename);
            }
            else{
                errorMessage += "文件"+filename+",";
                mark = 0;
            }
        }
        errorMessage +="不存在或已被移动";
    }
    return mark;
}

QString PlayList::get_error_message()
{
    return errorMessage;
}

int PlayList::length()
{
    return my_list.length();
}

QString& PlayList::operator[](int i)
{
    return my_list[i];
}

QString PlayList::get_listItem(int index)
{
    return my_list[index];
}

int PlayList::get_index(QString filename)
{
    return my_list.indexOf(filename);
}

void PlayList::write()
{
    //QFile file("./playlist.txt");
    QFile file(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/AppData/Local/FamilyPlayer/playlist.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QString res="";
    for(int i=0;i<my_list.length();i++){
        res = my_list[i]+'\n';
        file.write(res.toUtf8());
    }
    file.close();
}

int PlayList::add_file(QString filename)
{
    //是否是可播放资源
    QFileInfo fileinfo(filename);
    QString file_suffix = fileinfo.suffix();
    bool flag=false;
    for(int i=0;i<suffixList.length();i++){
        if(file_suffix==suffixList[i]){
            flag=true;
            break;
        }
    }
    if(!flag){
        return -1;
    }
    int currentIndex = my_list.indexOf(filename); //playlist中是否已有该文件
    //playlist长度为0或文件不在playlist中，添加进playlist中
    if(my_list.length()==0 || currentIndex == -1)
    {
        my_list.append(filename);
        return 1;
    }
    else
    {
        return 0;
    }
}

void PlayList::removeAt(int row)
{
    my_list.removeAt(row);
}

int PlayList::next_file(int current_index, int playmode)
{
    switch(playmode){
    case 1:{        //顺序播放
        return (current_index + 1) % (my_list.length());
    }
    case 2:{        //随机播放
        srand(QTime(0,0,0).secsTo(QTime::currentTime()));
        return rand()%(my_list.length());
    }
    case 3:{
        return current_index;
    }
    }
}

