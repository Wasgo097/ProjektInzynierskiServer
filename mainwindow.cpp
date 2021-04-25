#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serverinstance.h"
#include "Global.h"
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    _server_instance=ServerInstance::GetInstance(this);
    StartListener();
    StarManager();
}
MainWindow::~MainWindow(){
    StopListener();
    StopManager();
    delete ui;
}
void MainWindow::StartListener(){
    if(_server_instance!=nullptr)
        _server_instance->StartListener();
}
void MainWindow::StopListener(){
    if(_server_instance!=nullptr)
        _server_instance->StopListener();
}
void MainWindow::StarManager(){
    if(_server_instance!=nullptr)
        _server_instance->StartDatabase();
}
void MainWindow::StopManager(){
    if(_server_instance!=nullptr)
        _server_instance->StopDatabase();
}
