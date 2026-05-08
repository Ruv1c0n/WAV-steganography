#include "wavfile.h"

#include <QFile>

bool WavFile::load(const QString &path)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
        return false;

    header = file.read(44);

    if (header.size() != 44)
        return false;

    channels =
        *reinterpret_cast<int16_t*>(
            header.data() + 22);

    sampleRate =
        *reinterpret_cast<int32_t*>(
            header.data() + 24);

    bitsPerSample =
        *reinterpret_cast<int16_t*>(
            header.data() + 34);

    QByteArray audioData = file.readAll();

    int sampleCount =
        audioData.size() / sizeof(int16_t);

    samples.resize(sampleCount);

    memcpy(samples.data(),
           audioData.data(),
           audioData.size());

    file.close();

    filePath = path;

    return true;
}

bool WavFile::save(const QString &path)
{
    QFile file(path);

    if (!file.open(QIODevice::WriteOnly))
        return false;

    file.write(header);

    file.write(
        reinterpret_cast<char*>(
            samples.data()),
        samples.size() * sizeof(int16_t));

    file.close();

    return true;
}

bool WavFile::isValidFormat() const
{
    return channels == 1
           && sampleRate == 44100
           && bitsPerSample == 16;
}

QString WavFile::getInfo() const
{
    return QString(
               "Channels: %1\n"
               "Sample Rate: %2 Hz\n"
               "Bits Per Sample: %3\n"
               "Samples Count: %4")
        .arg(channels)
        .arg(sampleRate)
        .arg(bitsPerSample)
        .arg(samples.size());
}