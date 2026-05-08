#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "wavfile.h"
#include "audioplayer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:

    void onLoadClicked();

    void onEncodeClicked();

    void onDecodeClicked();

    void onSaveClicked();

    void onPlayClicked();

private:
    Ui::MainWindow *ui;

    WavFile wav;

    AudioPlayer player;
};

#endif // MAINWINDOW_H