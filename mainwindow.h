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

private:
    Ui::MainWindow *ui;
    QString inputFileName;
    QString ouputFileName;
};

#endif // MAINWINDOW_H
