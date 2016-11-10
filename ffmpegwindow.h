/**
  * Project: Labo 02 IHM
  * Authors: Antoine Drabble & Guillaume Serneels
  * Date: 28.11.2016
  */
#ifndef FfmpegWindow_H
#define FfmpegWindow_H

#include <QMainWindow>

namespace Ui {
class FfmpegWindow;
}

/**
 * @brief The FFMPEG main window
 */
class FfmpegWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Create the FFMPEG window
     * @param parent
     */
    explicit FfmpegWindow(QWidget *parent = 0);

    /**
     * @brief Destroy the FFMPEG window
     */
    ~FfmpegWindow();

private slots:
    /**
     * @brief Handle file menu quit button click to exit the application
     */
    void on_actionQuit_triggered();

    /**
     * @brief Show file input selector window and update the command
     */
    void on_selectInputFile_clicked();

    /**
     * @brief Show file output selector window and update the command
     */
    void on_selectOutputFile_clicked();

    /**
     * @brief Update the command when the output file input is manually changed
     * @param arg1
     */
    void on_inputOutputFile_textChanged(const QString &arg1);

    /**
     * @brief Update the command shown in the bottom text browser
     */
    void updateCommand();

    /**
     * @brief Update command when slider changes and don't allow start time slider
     *        to have a higher value thant the end time slider
     * @param the new value of the slider
     */
    void on_sliderStartTime_valueChanged(int value);

    /**
     * @brief Update command when slider changes and don't allow end time slider
     *        to have a lower value thant the start time slider
     * @param the new value of the slider
     */
    void on_sliderEndTime_valueChanged(int value);

    /**
     * @brief Copy the command output to the clipboard when clicked
     */
    void on_copyToClipboard_clicked();

    /**
     * @brief Convert microseconds to a formatted string
     * @param the millliseconds
     * @return the formatted string
     */
    QString msToString(quint64 ms);


private:
    Ui::FfmpegWindow *ui;   // The UI containing all the fields
    QString inputFileName,  // The input file name
            outputFileName; // The output file name
    quint64  startTime,     // The slider start time to cut the video in ms
             endTime,       // The slider end time to cut the video in ms
             duration;      // The duration of the video in ms
};

#endif // FfmpegWindow_H
