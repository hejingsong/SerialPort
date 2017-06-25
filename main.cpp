/**
 * 串口助手
 * Version：1.0
 * Author： Hejs
 * Email:   240197153@qq.com
 */
#include "config.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SerialPort w;
    w.show();
    SystemPort port(&w);
    w.bindPort(&port);

    return a.exec();
}
