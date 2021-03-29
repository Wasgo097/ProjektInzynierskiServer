#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "udplistener.h"
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    StartListening();
}
MainWindow::~MainWindow(){
    StopListening();
    delete ui;
}

void MainWindow::StartListening()
{
    Listener=std::shared_ptr<UdpListener>(new UdpListener(this));
    Listener->start();
}

void MainWindow::StopListening()
{
    if(Listener){
        Listener->quit();
        Listener.reset();
    }
}
