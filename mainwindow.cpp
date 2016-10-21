#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QProcess>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QProcess process;
    process.start("ffprobe");
    process.waitForFinished(-1);
    QString output(process.readAllStandardOutput());
    qDebug() << output;
    ui->textBrowserFileProperties->setText(output);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    qApp->quit();
}

void MainWindow::on_selectInputFile_clicked()
{
    inputFileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString());

    if (!inputFileName.isEmpty()) {
        /*QFile file(inputFileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"), tr("AVI files (*.avi);; MP4 Files (*.mp4)"));
            return;
        }
        file.close();*/


        ui->inputInputFile->setText(inputFileName);

        QProcess process;
        process.start("ffprobe", QStringList() << "-v error -show_format -show_streams " << inputFileName);
        process.waitForFinished(-1);
        QString output(process.readAllStandardOutput());
        qDebug() << output;
        ui->textBrowserFileProperties->setText(output);
    }
}

void MainWindow::on_selectOutputFile_clicked()
{
    ouputFileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString());

    if (!ouputFileName.isEmpty()) {
        QFile file(ouputFileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"), tr("AVI files (*.avi);; MP4 Files (*.mp4)"));
            return;
        }
        ui->inputOutputFile->setText(ouputFileName);
        file.close();
    }
}
