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

    for (int i = 0; i < 32; i++)
    {
        int bit = (size >> i) & 1;

        samples[i] =
            (samples[i] & ~1) | bit;
    }

    int sampleIndex = 32;

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
    uint32_t size = 0;

    for (int i = 0; i < 32; i++)
    {
        size |=
            (samples[i] & 1) << i;
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