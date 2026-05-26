#ifndef WAVFILE_H
#define WAVFILE_H

#include <QString>
#include <QVector>
#include <QByteArray>

/*
 * Класс для работы с WAV-файлами формата PCM.
 * Отвечает за парсинг заголовка, чтение/запись аудиоданных и проверку формата.
*/
class WavFile
{
public:
    bool load(const QString& path);

    bool save(const QString& path);

    bool isValidFormat() const;

    QString getInfo() const;

    QVector<int16_t> samples;

    QString filePath;

    int channels = 0;
    int sampleRate = 0;
    int bitsPerSample = 0;

private:
    QByteArray header;
};

#endif // WAVFILE_H