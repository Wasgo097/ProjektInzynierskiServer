#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "udplistener.h"
#include "dbmanager.h"
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    StartListening();
    DbManager=std::shared_ptr<DBManager>(new DBManager(this));
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
