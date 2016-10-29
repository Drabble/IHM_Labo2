#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QProcess>
#include <QDebug>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    // METTRE A JOUR inputFileName quand on edit à la main l'input field

    QString inputFileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString());

    if (!inputFileName.isEmpty()) {
        /*QFile file(inputFileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"), tr("AVI files (*.avi);;
                MP4 Files (*.mp4)..., See supported formats : https://ffmpeg.org/general.html"));
            return;
        }
        file.close();*/

        QProcess process1;
        process1.start("ffmpeg", QStringList() << "-v" << "error" << "-i" << inputFileName << "-f" << "null" << "-");
        if(process1.waitForStarted()){
            qDebug() << "Starting";
        }
        else{
            QMessageBox::critical(this, tr("Error"),
                                  tr("Error starting cmd.exe process"));
            qDebug() << process1.errorString();
        }
        process1.waitForFinished(-1);
        QString errorOutput(process1.readAllStandardError());
        qDebug() << errorOutput;
        QString output(process1.readAllStandardOutput());
        qDebug() << output;
        // Est-ce qu'on affiche le message d'erreur ffmpeg
        if(output.length() > 0 || errorOutput.length() > 0){
            QMessageBox::critical(this, tr("Error in the input file"),
                                  (errorOutput.length() > 0 ? errorOutput : "") + (output.length() > 0 ? output : ""));
        }

        ui->inputInputFile->setText(inputFileName);

        QProcess process2;
        process2.start("ffprobe", QStringList() << "-v" << "error" << "-show_format" << "-show_streams" << inputFileName);
        if(process2.waitForStarted()){
            qDebug() << "Starting";
        }
        else{
            ui->textBrowserFileProperties->setText("Error starting cmd.exe process");
                    qDebug() << process2.errorString();
            return;
        }
        process2.waitForFinished(-1);
        QString errorOutput2(process2.readAllStandardError());
        qDebug() << errorOutput2;
        if(errorOutput.length() > 0){
            ui->textBrowserFileProperties->setText("Error in the input file");
        } else{
            // Est-ce qu'on affiche le résultat de la commande ou juste la durée  ??
            QString output2(process2.readAllStandardOutput());
            qDebug() << output2;
            ui->textBrowserFileProperties->setText(output2);

            QProcess process3;
            process3.start("ffprobe", QStringList() << "-v" << "error" << "-show_entries" <<
                           "format=duration" << "-of" << "default=noprint_wrappers=1:nokey=1" << inputFileName);

            if(process3.waitForStarted()){
                qDebug() << "Starting";
            }
            else{
                ui->textBrowserFileProperties->setText("Error starting cmd.exe process");
                        qDebug() << process3.errorString();
                return;
            }
            process3.waitForFinished(-1);
            QString errorOutput3(process3.readAllStandardError());
            qDebug() << errorOutput3;
            if(errorOutput.length() > 0){
                ui->textBrowserFileProperties->setText("Error in the input file");
            } else{
                QString output3(process3.readAllStandardOutput());
                qDebug() << output3;
                duration = int(output3.toFloat());

                qDebug() << "Duration : " << duration;
                ui->sliderStartTime->setMaximum(duration);
                ui->sliderEndTime->setMaximum(duration);
                ui->sliderEndTime->setValue(duration);

                ui->labelStartTimeValue->setText("00:00:00");
                ui->labelEndTimeValue->setText(QString::number(duration));

                this->inputFileName = inputFileName;
                updateCommand();
            }
        }
    }
}

void MainWindow::on_selectOutputFile_clicked()
{
    QString outputFileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString());

    if (!outputFileName.isEmpty()) {
        QFile file(outputFileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"), tr("AVI files (*.avi);; MP4 Files (*.mp4)"));
            return;
        }
        ui->inputOutputFile->setText(outputFileName);
        file.close();

        this->outputFileName = outputFileName;

        updateCommand();
    }
}

void MainWindow::updateCommand(){
    // BOUTON COPY TO CLIPBOARD
    if(inputFileName.length() > 0 && outputFileName.length() > 0){
        int duration = endTime - startTime;
        ui->textBrowserFfmpegCommand->setText("ffmpeg -i " + inputFileName + " -ss " + QDateTime::fromTime_t(startTime).toUTC().toString("hh:mm:ss") + ".0 -c copy -t " + QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss") + ".0 " + outputFileName);
    }
}

void MainWindow::on_sliderStartTime_valueChanged(int value)
{
    // Durée avec ms ??????????????
    if(inputFileName.length() > 0){
        ui->sliderStartTime->setValue(ui->sliderEndTime->value() < value ? ui->sliderEndTime->value() : value);
        ui->labelStartTimeValue->setText(QDateTime::fromTime_t(ui->sliderStartTime->value()).toUTC().toString("hh:mm:ss"));
        startTime = ui->sliderStartTime->value();
        updateCommand();
    } else{
        ui->labelStartTimeValue->setText("00:00:00");
        ui->labelEndTimeValue->setText("00:00:00");
    }
}

void MainWindow::on_sliderEndTime_valueChanged(int value)
{
    if(inputFileName.length() > 0){
        ui->sliderEndTime->setValue(ui->sliderStartTime->value() > value ? ui->sliderStartTime->value() : value);
        ui->labelEndTimeValue->setText(QDateTime::fromTime_t(ui->sliderEndTime->value()).toUTC().toString("hh:mm:ss"));
        endTime = ui->sliderEndTime->value();
        updateCommand();
    } else{
        ui->labelStartTimeValue->setText("00:00:00");
        ui->labelEndTimeValue->setText("00:00:00");
    }
}
