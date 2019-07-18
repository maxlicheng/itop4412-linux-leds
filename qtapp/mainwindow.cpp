/**********************************************************************************************************
** 文件名		:mainvindow.cpp
** 作者			:maxlicheng<licheng.chn@outlook.com>
** 作者github	:https://github.com/maxlicheng
** 作者博客		:https://www.maxlicheng.com/	
** 生成日期		:2019-07-18
** 描述			:iTop4412全能板led 	qt应用程序 	qt版本v5.7.0
************************************************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <errno.h>
#include <string.h>

#define LED_IOC_SET  		_IO ('k', 0)
#define LED_IOC_GET 		_IO ('k', 1)
#define LED_IOC_SET_BLINK 	_IO ('k', 2)
#define LED_IOC_GET_BLINK	_IO ('k', 3)

static int fb1, fb2,bell_fb;
volatile int led1_sta = 0, led2_sta = 0;
volatile int bell_status = 0;

static int adc_fd;
const char *adc = "/dev/adc";
char buff[30] = "";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    const char *led0 = "/dev/led0";
    const char *led1 = "/dev/led1";

    ui->setupUi(this);

    connect(ui->led1_cbox, SIGNAL(toggled(bool)), this, SLOT(led1_toggled()));
    connect(ui->led2_cbox, SIGNAL(toggled(bool)), this, SLOT(led2_toggled()));
    connect(ui->led1_cboBox, SIGNAL(activated(QString)), this, SLOT(on_led1_cboBox_activated(QString)));
    connect(ui->led2_cboBox, SIGNAL(activated(QString)), this, SLOT(on_led2_cboBox_activated(QString)));

    connect(ui->start_btn, SIGNAL(clicked()), this, SLOT(on_start_btn_clicked()));
    connect(ui->stop_btn, SIGNAL(clicked()), this, SLOT(on_stop_btn_clicked()));

    connect(ui->bell_cBox, SIGNAL(toggled(bool)), this, SLOT(bell_toggled()));

    connect(ui->quit_btn, SIGNAL(clicked(bool)), this, SLOT(close()));

    ui->textBrowser->document()->setMaximumBlockCount(50);      //最多显示50行

    if((fb1 = open(led0, O_RDWR|O_NOCTTY|O_NDELAY))<0){
        printf("open %s failed\n",led0);
        exit(1);
    }
    if((fb2 = open(led1, O_RDWR|O_NOCTTY|O_NDELAY))<0){
        printf("open %s failed\n",led1);
        exit(1);
    }
    if((bell_fb = open("/dev/buzzer_ctl", O_RDWR|O_NOCTTY|O_NDELAY))<0){
        printf("open %s failed\n","/dev/buzzer_ctl");
        exit(1);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::led1_toggled()
{
    led1_sta = ~led1_sta;
    ioctl(fb1, LED_IOC_SET, led1_sta&0x01);
}


void MainWindow::on_led1_cboBox_activated(const QString &arg1)
{
    ioctl(fb1, LED_IOC_SET_BLINK, arg1.toInt());
}



void MainWindow::on_getParam1_currentIndexChanged(const QString &arg1)
{
    int varg;

//    ui->textBrowser->clear();

    if(!QString::compare("getSta", arg1)){
       ioctl(fb1, LED_IOC_GET, &varg);
       ui->textBrowser->append("led1_sta:"+QString::number(varg));
    }
    else if(!QString::compare("getFrq", arg1)){
        ioctl(fb1, LED_IOC_GET_BLINK, &varg);
        ui->textBrowser->append("led1_freq:"+QString::number(varg));
    }
}

void MainWindow::led2_toggled()
{
    led2_sta = ~led2_sta;
    ioctl(fb2, LED_IOC_SET, led2_sta&0x01);
}

void MainWindow::on_led2_cboBox_activated(const QString &arg1)
{
    ioctl(fb2, LED_IOC_SET_BLINK, arg1.toInt());
}

void MainWindow::on_getParam2_currentIndexChanged(const QString &arg1)
{
    int varg;

//    ui->textBrowser->clear();

    if(!QString::compare("getSta", arg1)){
       ioctl(fb2, LED_IOC_GET, &varg);
       ui->textBrowser->append("led2_sta:"+QString::number(varg));
    }
    else if(!QString::compare("getFrq", arg1)){
        ioctl(fb2, LED_IOC_GET_BLINK, &varg);
        ui->textBrowser->append("led2_freq:"+QString::number(varg));
    }
}

void MainWindow::bell_toggled()
{
    bell_status = ~bell_status;
    ioctl(bell_fb,bell_status&0x01,0);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == this->timer_1s){

        int len = ::read(adc_fd, buff, sizeof(buff)-1);
        if(len){
            buff[len] = '\0';
            int value = -1;
            sscanf(buff, "%d", &value);
            ui->lcdNumber->display(value);
        }
    }
}

void MainWindow::on_start_btn_clicked()
{
    if((adc_fd = ::open(adc, O_RDWR|O_NOCTTY|O_NDELAY))<0){
        printf("open %s failed\n",adc);
        exit(1);
    }
    this->timer_1s = this->startTimer(500);
}

void MainWindow::on_stop_btn_clicked()
{
    ui->lcdNumber->display(0);
    ::close(adc_fd);
    this->killTimer(this->timer_1s);
}
