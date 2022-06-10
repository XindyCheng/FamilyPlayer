#include "dialog.h"
#include "ui_dialog.h"

#pragma execution_character_set("utf-8")
Dialog::Dialog(QWidget *parent) : FramelessDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->initForm();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::initForm()
{
    //设置标题栏控件
    ui->labTitle->setText("媒体信息");
    this->setWindowTitle(ui->labTitle->text());
    this->setTitleBar(ui->labTitle);

    //关联信号
    connect(this, SIGNAL(titleDblClick()), this, SLOT(titleDblClick()));
    connect(this, SIGNAL(windowStateChange(bool)), this, SLOT(windowStateChange(bool)));
}

void Dialog::titleDblClick()
{
    on_btnMenu_Max_clicked();
}

void Dialog::windowStateChange(bool max)
{
    ui->btnMenu_Max->setIcon(max ? QPixmap(":/images/arrows-shrink.PNG"): QPixmap(":/images/arrows-expand.PNG"));
    ui->btnMenu_Max->setToolTip(max ? "还原" : "最大化");
}

void Dialog::on_btnMenu_Min_clicked()
{
    this->showMinimized();
}

void Dialog::on_btnMenu_Max_clicked()
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

void Dialog::on_btnMenu_Close_clicked()
{
    this->close();
}

void Dialog::setList(QStringList metadata){
    ui->tableWidget->setStyleSheet("background-color:rgba(0,0,0,0);color:white;");
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->horizontalHeader()->hide();
    ui->tableWidget->verticalHeader()->hide();

    //ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableWidget->setRowCount(9);
    ui->tableWidget->setColumnCount(2);
    QStringList metainfo;
    metainfo<<"文件名： "<<"视频码率： "<<"视频帧率： "<<"视频编码方式： "<<"音频码率： "<<"音频编码方式： "<<"音频声道数： "<<"专辑标题： "<<"演唱者： ";
    for(int row=0;row<9;row++){
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(metainfo[row]));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(metadata[row]));
    }
}
