#pragma execution_character_set("utf-8")

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);

    QApplication a(argc, argv);


    QString qss;
        QFile qssFile(":/myqss.qss");
        qssFile.open(QFile::ReadOnly);

        if(qssFile.isOpen())
            {
                qss = QLatin1String(qssFile.readAll());
                qApp->setStyleSheet(qss);
                qssFile.close();
            }

    MainWindow *window = new MainWindow;
    window->show();
    return a.exec();
}

