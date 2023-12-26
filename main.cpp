#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <thread>
#include <QApplication>
#include "mainwindow.h"




int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
