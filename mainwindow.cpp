#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serverinstance.h"
#include "Global.h"
#include "logcontainer.h"
#include "logutilities.h"
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    ui->LogType->addItem("All Logs");
    ui->LogType->addItem("UdpListener Logs");
    ui->LogType->addItem("SerialListener Logs");
    ui->LogType->addItem("Server Logs");
    ui->LogType->addItem("DB Manager Logs");
}
MainWindow::~MainWindow(){
    StopManager();
    StopListeners();
    delete ui;
}
void MainWindow::StartListeners(QString serialport){
    if(_server_instance!=nullptr){
        _server_instance->StartListeners(serialport);
    }
}
void MainWindow::StopListeners(){
    if(_server_instance!=nullptr){
        _server_instance->StopListeners();
    }
}
void MainWindow::StarManager(){
    if(_server_instance!=nullptr){
        _server_instance->StartDatabase();
    }
}
void MainWindow::StopManager(){
    if(_server_instance!=nullptr){
        _server_instance->StopDatabase();
    }
}
void MainWindow::on_StartBtn_clicked(){
    QString serialport=ui->txt_serialport->toPlainText();
    _server_instance=ServerInstance::GetInstance(this,serialport);
    StartListeners(serialport);
    StarManager();
}
void MainWindow::on_SaveLogBtn_clicked(){
    int index=ui->LogType->currentIndex();
    if(index>=0){
        LogType type=LogType(index);
        LogContainer::GetInstance()->SaveLog(type);
    }
}
void MainWindow::on_StopBtn_clicked(){
//    StopManager();
//    StopListeners();
}
