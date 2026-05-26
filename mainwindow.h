#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QTemporaryFile>

#include "wavfile.h"

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
    void onCopyClicked();

    void onPlayOriginalClicked();
    void onPlayEncodedClicked();

    void updateOriginalProgress(qint64 position);
    void updateEncodedProgress(qint64 position);
    void onOriginalDurationChanged(qint64 duration);
    void onEncodedDurationChanged(qint64 duration);

    void onOriginalStateChanged(QMediaPlayer::PlaybackState state);
    void onEncodedStateChanged(QMediaPlayer::PlaybackState state);

    // Слоты для обработки перемотки аудио ползунками (слайдерами)
    void onOriginalSliderMoved(int position);
    void onEncodedSliderMoved(int position);

    void onOriginalSliderPressed();
    void onOriginalSliderReleased();
    void onEncodedSliderPressed();
    void onEncodedSliderReleased();

    // ДОБАВЛЕНО:
    void onHelpClicked();

private:
    Ui::MainWindow *ui;

    WavFile wav;

    QMediaPlayer* originalPlayer;
    QAudioOutput* originalOutput;

    QMediaPlayer* encodedPlayer;
    QAudioOutput* encodedOutput;

    QTemporaryFile* tempEncodedFile;

    QString originalSourcePath;
    QString encodedSourcePath;

    bool isEncoded;

    // Флаги блокировки: если пользователь тянет ползунок,
    // отключаем автоматическое обновление позиции от плеера, чтобы они не "сопротивлялись" друг другу.
    bool isSeekingOriginal;
    bool isSeekingEncoded;

    void resetPlayers();
};

#endif // MAINWINDOW_H