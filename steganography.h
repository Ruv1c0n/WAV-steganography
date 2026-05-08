#ifndef STEGANOGRAPHY_H
#define STEGANOGRAPHY_H

#include <QString>
#include <QVector>

class Steganography
{
public:
    static bool encode(
        QVector<int16_t>& samples,
        const QString& message);

    static QString decode(
        const QVector<int16_t>& samples);
};

#endif // STEGANOGRAPHY_H