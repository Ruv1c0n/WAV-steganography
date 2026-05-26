#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "steganography.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <QTimer>
#include <QUrl>
#include <QStyle>
#include <QFileInfo>
#include <QMenuBar>
#include <QAction>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    isEncoded = false;

    originalPlayer = new QMediaPlayer(this);
    originalOutput = new QAudioOutput(this);
    originalPlayer->setAudioOutput(originalOutput);

    encodedPlayer = new QMediaPlayer(this);
    encodedOutput = new QAudioOutput(this);
    encodedPlayer->setAudioOutput(encodedOutput);

    tempEncodedFile = nullptr;

    // Подключение кнопок
    connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::onLoadClicked);
    connect(ui->encodeButton, &QPushButton::clicked, this, &MainWindow::onEncodeClicked);
    connect(ui->decodeButton, &QPushButton::clicked, this, &MainWindow::onDecodeClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    connect(ui->copyButton, &QPushButton::clicked, this, &MainWindow::onCopyClicked);

    connect(ui->playOriginalButton, &QPushButton::clicked, this, &MainWindow::onPlayOriginalClicked);
    connect(ui->playEncodedButton, &QPushButton::clicked, this, &MainWindow::onPlayEncodedClicked);

    // Подключение слайдеров: sliderMoved для перемотки,
    // sliderPressed/Released для управления флагами блокировки (isSeeking)
    connect(ui->sliderOriginal, &QSlider::sliderMoved, this, &MainWindow::onOriginalSliderMoved);
    connect(ui->sliderEncoded, &QSlider::sliderMoved, this, &MainWindow::onEncodedSliderMoved);

    // ДОБАВЛЕНО: Захват и отпускание ползунка
    connect(ui->sliderOriginal, &QSlider::sliderPressed, this, &MainWindow::onOriginalSliderPressed);
    connect(ui->sliderOriginal, &QSlider::sliderReleased, this, &MainWindow::onOriginalSliderReleased);
    connect(ui->sliderEncoded, &QSlider::sliderPressed, this, &MainWindow::onEncodedSliderPressed);
    connect(ui->sliderEncoded, &QSlider::sliderReleased, this, &MainWindow::onEncodedSliderReleased);

    // Обновление ползунков
    connect(originalPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::updateOriginalProgress);
    connect(originalPlayer, &QMediaPlayer::durationChanged, this, &MainWindow::onOriginalDurationChanged);

    connect(encodedPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::updateEncodedProgress);
    connect(encodedPlayer, &QMediaPlayer::durationChanged, this, &MainWindow::onEncodedDurationChanged);

    // Смена состояний плеера
    connect(originalPlayer, &QMediaPlayer::playbackStateChanged, this, &MainWindow::onOriginalStateChanged);
    connect(encodedPlayer, &QMediaPlayer::playbackStateChanged, this, &MainWindow::onEncodedStateChanged);

    // Приветственное сообщение в статусбаре
    ui->statusBar->showMessage("Готово к работе. Загрузите WAV файл.");

    // Инициализация флагов
    isSeekingOriginal = false;
    isSeekingEncoded = false;

    // Подключение меню
    // Файл
    connect(ui->Upload, &QAction::triggered, this, &MainWindow::onLoadClicked);
    connect(ui->Save, &QAction::triggered, this, &MainWindow::onSaveClicked);
    connect(ui->Quit, &QAction::triggered, this, &QMainWindow::close);

    // Помощь
    connect(ui->helpAction, &QAction::triggered, this, &MainWindow::onHelpClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (tempEncodedFile) {
        tempEncodedFile->remove();
        delete tempEncodedFile;
    }
}

void MainWindow::onLoadClicked()
{
    resetPlayers();

    QString path = QFileDialog::getOpenFileName(this, "Open WAV", "", "*.wav");
    if (path.isEmpty()) return;

    if (!wav.load(path)) {
        QMessageBox::warning(this, "Error", "Cannot load WAV");
        return;
    }

    ui->sourceLine->setText(path);
    originalSourcePath = path;
    isEncoded = false;

    ui->statusLabel->setText("");
    ui->encodeStatusLabel->setText("");
    ui->encodeStatusLabel->setVisible(false);

    ui->freqLabel->setText(QString("Частота: %1 Гц").arg(wav.sampleRate));
    ui->chanelLabel->setText(QString("Разрядность: %1 бит").arg(wav.bitsPerSample));
    ui->formatLabel->setText(QString("Каналы: %1").arg(wav.channels == 1 ? "моно" : "стерео"));

    double durationSec = (double)wav.samples.size() / wav.sampleRate;
    ui->durationLabel->setText(QString("Длительность: %1 сек").arg(QString::number(durationSec, 'f', 1)));

    if (wav.isValidFormat()) {
        ui->statusLabel->setText("✓ Формат поддерживается");
        ui->statusLabel->setStyleSheet("QObject { color: #27AE60 }");
        ui->statusBar->showMessage("Файл успешно загружен и готов к работе", 5000);
    } else {
        ui->statusLabel->setText("✗ Неверный формат!");
        ui->statusLabel->setStyleSheet("QObject { color: #E74C3C }");
        ui->statusBar->showMessage("Ошибка: Формат файла не поддерживается (нужен 16бит, Моно, 44100Гц)", 5000);
    }
}

void MainWindow::onEncodeClicked()
{
    if (!wav.isValidFormat()) {
        QMessageBox::warning(this, "Error", "Неверный формат WAV!");
        return;
    }

    QString message = ui->messageEdit->toPlainText();
    if (message.isEmpty()) {
        QMessageBox::warning(this, "Error", "Введите сообщение.");
        return;
    }

    ui->encodeStatusLabel->setVisible(false);

    if (!Steganography::encode(wav.samples, message)) {
        QMessageBox::warning(this, "Error", "Сообщение слишком большое!");
        return;
    }

    isEncoded = true;
    ui->encodeStatusLabel->setText("✓ Сообщение спрятано");
    ui->encodeStatusLabel->setVisible(true);
    ui->statusBar->showMessage("Сообщение успешно встроено в аудиофайл", 5000);
}

void MainWindow::onDecodeClicked()
{
    if (wav.samples.isEmpty()) {
        QMessageBox::warning(this, "Error", "Загрузите WAV файл.");
        return;
    }

    QString message = Steganography::decode(wav.samples);
    ui->decodedText->setPlainText(message);

    if (message.isEmpty()) {
        ui->decodedText->setPlainText("Сообщение не найдено или файл не содержит стеганограммы.");
        ui->statusBar->showMessage("Стеганограмма не обнаружена", 5000);
    } else {
        ui->statusBar->showMessage("Скрытое сообщение успешно извлечено", 5000);
    }
}

void MainWindow::onSaveClicked()
{
    if (wav.samples.isEmpty()) {
        QMessageBox::warning(this, "Error", "Нечего сохранять.");
        return;
    }

    QString path = QFileDialog::getSaveFileName(this, "Save WAV", "", "*.wav");
    if (path.isEmpty()) return;

    if (wav.save(path)) {
        ui->statusBar->showMessage("Файл успешно сохранен по пути: " + path, 5000);
    } else {
        QMessageBox::warning(this, "Error", "Ошибка сохранения.");
    }
}

void MainWindow::onCopyClicked()
{
    QString text = ui->decodedText->toPlainText();
    if (!text.isEmpty() && text != "Сообщение не найдено или файл не содержит стеганограммы.") {
        QApplication::clipboard()->setText(text);

        // Иконка галочки (успешное копирование)
        ui->copyButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogApplyButton));
        ui->statusBar->showMessage("Текст скопирован в буфер обмена", 3000);

        QTimer::singleShot(1000, [this]() {
            // Возвращаем иконку копирования
            ui->copyButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditCopy));
        });
    }
}

// --- Логика плеера (Play/Pause) ---

void MainWindow::onPlayOriginalClicked()
{
    if (originalSourcePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Сначала загрузите файл (нажмите 'Загрузить WAV').");
        return;
    }

    if (originalPlayer->playbackState() == QMediaPlayer::PlayingState) {
        originalPlayer->pause();
        return;
    }

    if (originalPlayer->playbackState() == QMediaPlayer::PausedState) {
        originalPlayer->play();
        return;
    }

    encodedPlayer->stop();
    originalPlayer->setSource(QUrl::fromLocalFile(originalSourcePath));
    originalPlayer->play();
}

void MainWindow::onPlayEncodedClicked()
{
    if (!isEncoded) {
        QMessageBox::warning(this, "Error", "Сначала внедрите сообщение (нажмите 'Спрятать'), чтобы получить файл со стеганограммой.");
        return;
    }

    if (encodedPlayer->playbackState() == QMediaPlayer::PlayingState) {
        encodedPlayer->pause();
        return;
    }

    if (encodedPlayer->playbackState() == QMediaPlayer::PausedState && !encodedSourcePath.isEmpty()) {
        encodedPlayer->play();
        return;
    }

    originalPlayer->stop();

    if (tempEncodedFile) {
        tempEncodedFile->remove();
        delete tempEncodedFile;
    }

    // Чтобы воспроизвести измененный файл из оперативной памяти,
    // сохраняем его во временный файл. QTemporaryFile удалится автоматически при закрытии.
    tempEncodedFile = new QTemporaryFile(QDir::tempPath() + "/stegoXXXXXX.wav");
    if (tempEncodedFile->open()) {
        encodedSourcePath = tempEncodedFile->fileName();
        tempEncodedFile->close();

        if (wav.save(encodedSourcePath)) {
            encodedPlayer->setSource(QUrl::fromLocalFile(encodedSourcePath));
            encodedPlayer->play();
        }
    }
}

// --- Перемотка ---

void MainWindow::onOriginalSliderPressed()
{
    isSeekingOriginal = true; // Пользователь взял ползунок - блокируем обновление от плеера
}

void MainWindow::onOriginalSliderReleased()
{
    isSeekingOriginal = false; // Пользователь отпустил ползунок - разблокируем
    originalPlayer->setPosition(ui->sliderOriginal->value()); // Перематываем на отпускание
}

void MainWindow::onOriginalSliderMoved(int position)
{
    if (isSeekingOriginal) {
        originalPlayer->setPosition(position); // Живая перемотка пока тянем
    }
}

void MainWindow::onEncodedSliderPressed()
{
    isSeekingEncoded = true;
}

void MainWindow::onEncodedSliderReleased()
{
    isSeekingEncoded = false;
    encodedPlayer->setPosition(ui->sliderEncoded->value());
}

void MainWindow::onEncodedSliderMoved(int position)
{
    if (isSeekingEncoded) {
        encodedPlayer->setPosition(position);
    }
}

// --- Обновление UI плеера ---

void MainWindow::updateOriginalProgress(qint64 position)
{
    // Обновляем ползунок только если пользователь его НЕ держит сейчас
    if (!isSeekingOriginal) {
        ui->sliderOriginal->setValue(position);
    }
}

void MainWindow::onOriginalDurationChanged(qint64 duration)
{
    ui->sliderOriginal->setRange(0, duration);
}

void MainWindow::updateEncodedProgress(qint64 position)
{
    if (!isSeekingEncoded) {
        ui->sliderEncoded->setValue(position);
    }
}

void MainWindow::onEncodedDurationChanged(qint64 duration)
{
    ui->sliderEncoded->setRange(0, duration);
}

// Смена иконок и статуса
void MainWindow::onOriginalStateChanged(QMediaPlayer::PlaybackState state)
{
    // Динамическая смена иконки: Play при остановке/паузе, Pause при воспроизведении.
    // Используем стандартные иконки Qt (QStyle), чтобы они работали на любой ОС.
    if (state == QMediaPlayer::PlayingState) {
        ui->playOriginalButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackPause));
        ui->statusBar->showMessage("Воспроизведение исходного файла...");
    } else {
        ui->playOriginalButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart));
        if (state == QMediaPlayer::StoppedState && originalPlayer->duration() > 0) {
            ui->statusBar->showMessage("Воспроизведение завершено", 3000);
        }
    }
}

void MainWindow::onEncodedStateChanged(QMediaPlayer::PlaybackState state)
{
    // Динамическая смена иконки: Play при остановке/паузе, Pause при воспроизведении.
    // Используем стандартные иконки Qt (QStyle), чтобы они работали на любой ОС.
    if (state == QMediaPlayer::PlayingState) {
        ui->playEncodedButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackPause));
        ui->statusBar->showMessage("Воспроизведение файла со стеганограммой...");
    } else {
        ui->playEncodedButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart));
        if (state == QMediaPlayer::StoppedState && encodedPlayer->duration() > 0) {
            ui->statusBar->showMessage("Воспроизведение завершено", 3000);
        }
    }
}

void MainWindow::resetPlayers()
{
    originalPlayer->stop();
    encodedPlayer->stop();
    originalSourcePath.clear();
    encodedSourcePath.clear();
    ui->sliderOriginal->setValue(0);
    ui->sliderEncoded->setValue(0);
}

void MainWindow::onHelpClicked()
{
    QMessageBox::about(this, "Инструкция пользователя",
                       "<h2>Стеганография WAV</h2>"
                       "<p>Приложение предназначено для скрытия текстовой информации в аудиофайлах формата WAV "
                       "с использованием метода LSB (младший значащий бит).</p>"
                       "<hr>"
                       "<h3>Ограничения формата</h3>"
                       "<p>Для корректной работы стеганографии исходный аудиофайл должен строго соответствовать параметрам:</p>"
                       "<ul>"
                       "  <li><b>Формат:</b> PCM (без сжатия)</li>"
                       "  <li><b>Частота дискретизации:</b> 44100 Гц</li>"
                       "  <li><b>Количество каналов:</b> 1 (Моно)</li>"
                       "  <li><b>Разрядность:</b> 16 бит</li>"
                       "</ul>"
                       "<h3>Как скрыть сообщение</h3>"
                       "<ol>"
                       "  <li>Нажмите <b>Загрузить WAV</b> и выберите исходный аудиофайл.</li>"
                       "  <li>Убедитесь, что появился статус <i>\"✓ Формат поддерживается\"</i>.</li>"
                       "  <li>В поле <b>Встраивание сообщения</b> введите текст, который хотите спрятать.</li>"
                       "  <li>Нажмите кнопку <b>Спрятать</b>.</li>"
                       "  <li>Нажмите <b>Сохранить WAV</b>, чтобы сохранить измененный файл.</li>"
                       "</ol>"
                       "<h3>Как извлечь сообщение</h3>"
                       "<ol>"
                       "  <li>Загрузите аудиофайл, содержащий стеганограмму.</li>"
                       "  <li>В блоке <b>Извлечение сообщения</b> нажмите <b>Извлечь сообщение</b>.</li>"
                       "  <li>Текст появится в поле ниже. Вы можете скопировать его с помощью кнопки рядом.</li>"
                       "</ol>"
                       "<h3>Плеер</h3>"
                       "<p>Вы можете прослушивать исходный и измененный файлы с помощью плееров в нижней части окна. "
                       "Поддерживается пауза и перемотка с помощью ползунков.</p>"
                       );
}
