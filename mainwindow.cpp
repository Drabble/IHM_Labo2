#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QProcess>
#include <QDebug>
#include <QString>
#include <QClipboard>

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
    // METTRE A JOUR inputFileName quand on edit à la main l'input field, OUT oui¨!

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
        qDebug() << "OUTPUT : " << "ffmpeg" << "-v" << "error" << "-i" << inputFileName << "-f" << "null" << "-";
        qDebug() << QStringList() << "-v" << "error" << "-i" << inputFileName << "-f" << "null" << "-";
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
            // Est-ce qu'on affiche le résultat de la commande ou juste la durée  ?? ou on met en forme ??
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
                duration = quint64(output3.toFloat() * 1000);

                qDebug() << "Duration : " << duration;
                ui->sliderStartTime->setMaximum(duration);
                ui->sliderEndTime->setMaximum(duration);
                ui->sliderStartTime->setValue(0);
                startTime = 0;
                ui->sliderEndTime->setValue(duration);
                endTime = duration;

                ui->labelStartTimeValue->setText("00:00:00.000");
                ui->labelEndTimeValue->setText(msToString(duration));

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
    if(inputFileName.length() > 0 && outputFileName.length() > 0){
        int duration = endTime - startTime;
        ui->textBrowserFfmpegCommand->setText("ffmpeg -i " + inputFileName + " -ss " + msToString(startTime) + " -c copy -t " + msToString(duration) + " " + outputFileName);
    }
}

void MainWindow::on_sliderStartTime_valueChanged(int value)
{
    if(inputFileName.length() > 0){
        ui->sliderStartTime->setValue(ui->sliderEndTime->value() < value ? ui->sliderEndTime->value() : value);
        ui->labelStartTimeValue->setText(msToString(ui->sliderStartTime->value()));
        startTime = ui->sliderStartTime->value();
        updateCommand();
    } else{
        ui->labelStartTimeValue->setText("00:00:00.000");
        ui->labelEndTimeValue->setText("00:00:00.000");
    }
}

void MainWindow::on_sliderEndTime_valueChanged(int value)
{
    if(inputFileName.length() > 0){
        ui->sliderEndTime->setValue(ui->sliderStartTime->value() > value ? ui->sliderStartTime->value() : value);
        ui->labelEndTimeValue->setText(msToString(ui->sliderEndTime->value()));
        endTime = ui->sliderEndTime->value();
        updateCommand();
    } else{
        ui->labelStartTimeValue->setText("00:00:00.000");
        ui->labelEndTimeValue->setText("00:00:00.000");
    }
}

QString MainWindow::msToString(quint64 ms){
    quint64 milliseconds = (quint64) (ms%1000);
    quint64 seconds = (quint64) (ms / 1000) % 60 ;
    quint64 minutes = (quint64) ((ms / (1000*60)) % 60);
    quint64 hours   = (quint64) ((ms / (1000*60*60)) % 24);
    return QString("%1:%2:%3.%4").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')).arg(milliseconds, 3, 10, QChar('0'));
}

void MainWindow::on_copyToClipboard_clicked()
{
    QClipboard *clip = QApplication::clipboard();
    QString input = ui->textBrowserFfmpegCommand->toPlainText();
    clip->setText(input);
}

void MainWindow::on_inputOutputFile_textChanged(const QString &arg1)
{
    outputFileName = arg1;
    updateCommand();
}
