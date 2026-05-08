#include "audioplayer.h"

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)
{
    player = new QMediaPlayer(this);

    audioOutput = new QAudioOutput(this);

    player->setAudioOutput(audioOutput);

    audioOutput->setVolume(1.0);
}

void AudioPlayer::play(const QString &path)
{
    player->setSource(QUrl::fromLocalFile(path));

    player->play();
}

void AudioPlayer::stop()
{
    player->stop();
}