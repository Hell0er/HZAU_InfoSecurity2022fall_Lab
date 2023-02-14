#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QMessageBox::information(&w,"提示","欢迎使用本3DES加解密小程序\n\n请输入16位的密钥key和初始向量IV！");
    w.setWindowTitle("3DES加解密小程序");
    w.show();
    return a.exec();
}
