#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();

    void on_selectInputFile_clicked();

    void on_selectOutputFile_clicked();

    void updateCommand();

    void on_sliderStartTime_valueChanged(int value);

    void on_sliderEndTime_valueChanged(int value);

    QString msToString(quint64 ms);

    void on_copyToClipboard_clicked();

    void on_inputOutputFile_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QString inputFileName,
            outputFileName;
    quint64  startTime,
             endTime,
             duration;
};

#endif // MAINWINDOW_H
