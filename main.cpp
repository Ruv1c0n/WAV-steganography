#include "mainwindow.h"
#include <QApplication>
#include <QIcon> // ДОБАВЛЕНО

int main(int argc, char *argv[])
{
    qputenv("QT_DEBUG_PLUGINS", "1");
    QApplication a(argc, argv);
    qDebug() << "App dir:" << QCoreApplication::applicationDirPath();
    qDebug() << "Lib paths:" << QCoreApplication::libraryPaths();
    QCoreApplication::addLibraryPath(
        QCoreApplication::applicationDirPath()
    );

    // Устанавливаем иконку для всего приложения (отобразится в окне и на панели задач)
    // Путь :/ означает обращение к файлу ресурсов .qrc
    a.setWindowIcon(QIcon(":/app_icon.png"));

    MainWindow w;
    w.show();

    return a.exec();
}