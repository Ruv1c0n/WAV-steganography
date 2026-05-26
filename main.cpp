#include "mainwindow.h"
#include <QApplication>
#include <QIcon> // ДОБАВЛЕНО

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Устанавливаем иконку для всего приложения (отобразится в окне и на панели задач)
    // Путь :/ означает обращение к файлу ресурсов .qrc
    a.setWindowIcon(QIcon(":/app_icon.png"));

    MainWindow w;
    w.show();

    return a.exec();
}