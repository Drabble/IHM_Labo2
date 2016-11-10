/**
  * Project: Labo 02 IHM
  * Authors: Antoine Drabble & Guillaume Serneels
  * Date: 28.11.2016
  */
#include "ffmpegwindow.h"
#include "ui_FfmpegWindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QProcess>
#include <QDebug>
#include <QString>
#include <QClipboard>
#include <QFileInfo>

FfmpegWindow::FfmpegWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FfmpegWindow)
{
    ui->setupUi(this);
}

FfmpegWindow::~FfmpegWindow()
{
    delete ui;
}

void FfmpegWindow::on_actionQuit_triggered()
{
    // Quit the app
    qApp->quit();
}

void FfmpegWindow::on_selectInputFile_clicked()
{
    // Show the dialog to input the filename
    QString inputFileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString());

    // If a file has been selected
    if (!inputFileName.isEmpty()) {

        // Run a ffmpeg to check if the file contains an error
        QProcess process1;
        qDebug() << "OUTPUT : " << "ffmpeg" << "-v" << "error" << "-i" << inputFileName << "-f" << "null" << "-";
        qDebug() << QStringList() << "-v" << "error" << "-i" << inputFileName << "-f" << "null" << "-";
        process1.start("ffmpeg", QStringList() << "-v" << "error" << "-i" << inputFileName << "-f" << "null" << "-");

        // Wait for the ffmpeg command to start
        if(process1.waitForStarted()){
            qDebug() << "Starting the command to check whether the file is valid or not";
        }
        else{
            QMessageBox::critical(this, tr("Error"),
                                  tr("Error starting cmd.exe process"));
            qDebug() << process1.errorString();
        }

        // Wait for the ffmpeg command to finish and read the output
        process1.waitForFinished(-1);
        QString errorOutput(process1.readAllStandardError());
        qDebug() << errorOutput;
        QString output(process1.readAllStandardOutput());
        qDebug() << output;

        // If any message is output, it means there is an error otherwise set the input file name
        if(output.length() > 0 || errorOutput.length() > 0){
            QMessageBox::critical(this, tr("Error in the input file"),
                                  (errorOutput.length() > 0 ? errorOutput : "") + (output.length() > 0 ? output : ""));
        } else{
            ui->inputInputFile->setText(inputFileName);
        }

        // Run the command to retrieve all the information about the video file
        QProcess process2;
        process2.start("ffprobe", QStringList() << "-v" << "error" << "-show_format" << "-show_streams" << inputFileName);

        // Wait for the command to start
        if(process2.waitForStarted()){
            qDebug() << "Starting command to retrieve the information about the video file";
        }
        else{
            ui->textBrowserFileProperties->setText("Error starting cmd.exe process");
            qDebug() << process2.errorString();
            return;
        }

        // Wait for the process to finish and retrieve the error output
        process2.waitForFinished(-1);
        QString errorOutput2(process2.readAllStandardError());
        qDebug() << errorOutput2;

        // If there is an error show the error in the file properties text browser
        if(errorOutput.length() > 0){
            ui->textBrowserFileProperties->setText("Error in the input file");
        } else{

            // Retrieve the command output and set the file properties text browser value
            QString output2(process2.readAllStandardOutput());
            qDebug() << output2;
            ui->textBrowserFileProperties->setText(output2);

            // Start the process to retrieve the output file duration
            QProcess process3;
            process3.start("ffprobe", QStringList() << "-v" << "error" << "-show_entries" <<
                           "format=duration" << "-of" << "default=noprint_wrappers=1:nokey=1" << inputFileName);

            // Wait for the process to start
            if(process3.waitForStarted()){
                qDebug() << "Starting process to retrieve the video file duration";
            }
            else{
                ui->textBrowserFileProperties->setText("Error starting cmd.exe process");
                qDebug() << process3.errorString();
                return;
            }

            // Wait for the process to finish and retrieve the error output
            process3.waitForFinished(-1);
            QString errorOutput3(process3.readAllStandardError());
            qDebug() << errorOutput3;

            // If there is an error with the command show the error in the file properties text browser
            if(errorOutput.length() > 0){
                ui->textBrowserFileProperties->setText("Error in the input file");
            } else{
                // Retrieve the command output and convert it to microseconds
                QString output3(process3.readAllStandardOutput());
                qDebug() << output3;
                duration = quint64(output3.toFloat() * 1000);
                qDebug() << "Video duration : " << duration;

                // Configure the sliders with the duration
                ui->sliderStartTime->setMaximum(duration);
                ui->sliderEndTime->setMaximum(duration);
                ui->sliderStartTime->setValue(0);
                ui->sliderEndTime->setValue(duration);
                ui->labelStartTimeValue->setText("00:00:00.000");
                ui->labelEndTimeValue->setText(msToString(duration));

                // Set the start time to 0 and end time to the duration
                startTime = 0;
                endTime = duration;

                // Update the input file name and the command to cut the video
                this->inputFileName = inputFileName;
                updateCommand();
            }
        }
    }
}

void FfmpegWindow::on_selectOutputFile_clicked()
{
    // Show the dialog to input the filename
    QString outputFileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString());

    // If a file has been selected
    if (!outputFileName.isEmpty()) {
        // Set the output file input text with the output file name
        ui->inputOutputFile->setText(outputFileName);

        // check if file exists
        QFileInfo check_file(outputFileName);
        if (check_file.exists() && check_file.isFile()) {
            ui->labelOutputFileExists->setText("The output file already exists, it will be overwritten !");
        } else {
            ui->labelOutputFileExists->setText("The output file doesn't exists, it will be created !");
        }

        // Update the output file name and update the command to cut the video
        this->outputFileName = outputFileName;
        updateCommand();
    } else{
        ui->labelOutputFileExists->setText("");
    }
}

void FfmpegWindow::on_inputOutputFile_textChanged(const QString &arg1)
{
    // Update the output file name and the command to cut the video
    outputFileName = arg1;
    updateCommand();


    // check if file exists
    if(outputFileName.length() > 0){
        QFileInfo check_file(outputFileName);
        if (check_file.exists() && check_file.isFile()) {
            ui->labelOutputFileExists->setText("The output file already exists, it will be overwritten !");
        } else {
            ui->labelOutputFileExists->setText("The output file doesn't exists, it will be created !");
        }
    } else{
        ui->labelOutputFileExists->setText("");
    }
}

void FfmpegWindow::on_sliderStartTime_valueChanged(int value)
{
    // If the input file is set,
    if(inputFileName.length() > 0){
        // Limit maximum value of the slider to the end time
        ui->sliderStartTime->setValue(ui->sliderEndTime->value() < value ? ui->sliderEndTime->value() : value);

        // Update the start time label with the start time
        ui->labelStartTimeValue->setText(msToString(ui->sliderStartTime->value()));

        // Update the start time and update the command to cut the video
        startTime = ui->sliderStartTime->value();
        updateCommand();
    }
    // Otherwise show 00:00:00.000 as the start time and end time
    else{
        ui->labelStartTimeValue->setText("00:00:00.000");
        ui->labelEndTimeValue->setText("00:00:00.000");
    }
}

void FfmpegWindow::on_sliderEndTime_valueChanged(int value)
{
    // If the input file is set,
    if(inputFileName.length() > 0){
        // Limit the minimum value of the slider to the start time
        ui->sliderEndTime->setValue(ui->sliderStartTime->value() > value ? ui->sliderStartTime->value() : value);

        // Update the end time label with the end time
        ui->labelEndTimeValue->setText(msToString(ui->sliderEndTime->value()));

        // Update the end time and update the command to cut the video
        endTime = ui->sliderEndTime->value();
        updateCommand();
    }
    // Otherwise show 00:00:00.000 as the start time and end time
    else{
        ui->labelStartTimeValue->setText("00:00:00.000");
        ui->labelEndTimeValue->setText("00:00:00.000");
    }
}

void FfmpegWindow::on_copyToClipboard_clicked()
{
    // Create the QClipboard object to save the command to the clipboard
    QClipboard *clip = QApplication::clipboard();

    // Retrieve the command and save it to the clipboard
    QString input = ui->textBrowserFfmpegCommand->toPlainText();
    clip->setText(input);
}

void FfmpegWindow::updateCommand(){
    // If the input file and output file are set
    if(inputFileName.length() > 0 && outputFileName.length() > 0){
        // Calculate the duration of the cut
        int duration = endTime - startTime;

        // Update the command in the ffmpeg cut command text browser
        ui->textBrowserFfmpegCommand->setText("ffmpeg -i " + inputFileName + " -ss " + msToString(startTime) + " -c copy -t " + msToString(duration) + " " + outputFileName);
    }
}

QString FfmpegWindow::msToString(quint64 ms){
    // Get the hours, minutes, seconds and millliseconds from the milliseconds parameter
    quint64 milliseconds = (quint64) (ms%1000);
    quint64 seconds = (quint64) (ms / 1000) % 60 ;
    quint64 minutes = (quint64) ((ms / (1000*60)) % 60);
    quint64 hours   = (quint64) ((ms / (1000*60*60)) % 24);

    // Format the calculated time and return it as a string
    return QString("%1:%2:%3.%4").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')).arg(milliseconds, 3, 10, QChar('0'));
}
