/**
  * Project: Labo 02 IHM
  * Authors: Antoine Drabble & Guillaume Serneels
  * Date: 28.11.2016
  */
#include "ffmpegwindow.h"
#include <QApplication>

/**
 * @brief Starts the ffmpeg window
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    // Create a new application
    QApplication a(argc, argv);

    // Show the Ffmpeg Window
    FfmpegWindow w;
    w.show();

    // Execute the appllication
    return a.exec();
}
