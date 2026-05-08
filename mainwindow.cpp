#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "steganography.h"

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->loadButton,
            &QPushButton::clicked,
            this,
            &MainWindow::onLoadClicked);

    connect(ui->encodeButton,
            &QPushButton::clicked,
            this,
            &MainWindow::onEncodeClicked);

    connect(ui->decodeButton,
            &QPushButton::clicked,
            this,
            &MainWindow::onDecodeClicked);

    connect(ui->saveButton,
            &QPushButton::clicked,
            this,
            &MainWindow::onSaveClicked);

    connect(ui->playButton,
            &QPushButton::clicked,
            this,
            &MainWindow::onPlayClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onLoadClicked()
{
    QString path =
        QFileDialog::getOpenFileName(
            this,
            "Open WAV",
            "",
            "*.wav");

    if (path.isEmpty())
        return;

    if (!wav.load(path))
    {
        QMessageBox::warning(
            this,
            "Error",
            "Cannot load WAV");

        return;
    }

    ui->infoLabel->setText(
        wav.getInfo());

    if (!wav.isValidFormat())
    {
        QMessageBox::warning(
            this,
            "Invalid Format",
            "Required:\n"
            "44100 Hz\n"
            "Mono\n"
            "16 bit PCM");
    }
}

void MainWindow::onEncodeClicked()
{
    if (!wav.isValidFormat())
    {
        QMessageBox::warning(
            this,
            "Error",
            "Invalid WAV format");

        return;
    }

    QString message =
        ui->messageEdit->toPlainText();

    if (!Steganography::encode(
            wav.samples,
            message))
    {
        QMessageBox::warning(
            this,
            "Error",
            "Message too large");

        return;
    }

    QMessageBox::information(
        this,
        "Success",
        "Message encoded");
}

void MainWindow::onDecodeClicked()
{
    QString message =
        Steganography::decode(
            wav.samples);

    ui->decodedEdit->setText(message);
}

void MainWindow::onSaveClicked()
{
    QString path =
        QFileDialog::getSaveFileName(
            this,
            "Save WAV",
            "",
            "*.wav");

    if (path.isEmpty())
        return;

    wav.save(path);
}

void MainWindow::onPlayClicked()
{
    player.play(wav.filePath);
}