#include "steganography.h"

bool Steganography::encode(
    QVector<int16_t>& samples,
    const QString& message)
{
    QByteArray data = message.toUtf8();

    uint32_t size = data.size();

    int requiredBits = 32 + size * 8;

    if (requiredBits > samples.size())
        return false;

    // Записываем размер сообщения (32 бита / 4 байта) в первые 32 сэмпла.
    // Это позволит при расшифровке точно знать, сколько байт нужно прочитать.
    for (int i = 0; i < 32; i++)
    {
        int bit = (size >> i) & 1;

        samples[i] =
            (samples[i] & ~1) | bit;
    }

    int sampleIndex = 32;

    // Побитово записываем само сообщение (каждый символ UTF-8 по 8 бит)
    // в младшие биты следующих сэмплов. Изменение 1/65536 не слышно человеку.
    for (char byte : data)
    {
        for (int bitIndex = 0;
             bitIndex < 8;
             bitIndex++)
        {
            int bit =
                (byte >> bitIndex) & 1;

            samples[sampleIndex] =
                (samples[sampleIndex] & ~1)
                | bit;

            sampleIndex++;
        }
    }

    return true;
}

QString Steganography::decode(
    const QVector<int16_t>& samples)
{
    // Защита: если файл слишком короткий, в нем точно ничего не спрятано
    if (samples.size() < 33) return "";

    // Считываем размер скрытого сообщения из первых 32 сэмплов
    uint32_t size = 0;

    for (int i = 0; i < 32; i++)
    {
        size |=
            (samples[i] & 1) << i;
    }

    // ЗАЩИТА ОТ ВЫЛЕТА: Если в обычном файле мусор в младших битах,
    // размер может считаться огромным. Проверяем, чтобы он не превышал
    // количество доступных сэмплов и был адекватным (до 1 МБ).
    int requiredSamples = 32 + size * 8;
    if (size == 0 || size > 1000000 || requiredSamples > samples.size()) {
        return ""; // Возвращаем пустую строку
    }

    QByteArray data;

    int sampleIndex = 32;

    for (uint32_t i = 0; i < size; i++)
    {
        char byte = 0;

        for (int bitIndex = 0;
             bitIndex < 8;
             bitIndex++)
        {
            byte |=
                (samples[sampleIndex] & 1)
                << bitIndex;

            sampleIndex++;
        }

        data.append(byte);
    }

    return QString::fromUtf8(data);
}