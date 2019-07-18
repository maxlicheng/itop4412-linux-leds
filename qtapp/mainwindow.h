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

protected:
    void timerEvent(QTimerEvent *);

private slots:

    void led1_toggled();

    void led2_toggled();

    void on_getParam1_currentIndexChanged(const QString &arg1);

    void on_led1_cboBox_activated(const QString &arg1);

    void on_led2_cboBox_activated(const QString &arg1);

    void on_getParam2_currentIndexChanged(const QString &arg1);

    void bell_toggled();

    void on_start_btn_clicked();

    void on_stop_btn_clicked();

private:
    Ui::MainWindow *ui;
    int timer_1s;
};

#endif // MAINWINDOW_H
