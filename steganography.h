#ifndef STEGANOGRAPHY_H
#define STEGANOGRAPHY_H

#include <QString>
#include <QVector>

/*
 * Класс для реализации стеганографии методом LSB (наименьший значащий бит).
 * Работает с массивами 16-битных аудиосэмплов.
*/
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