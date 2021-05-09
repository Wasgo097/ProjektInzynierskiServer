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
    StopManager();
    StopListener();
    delete ui;
}
void MainWindow::StartListener(){
    if(_server_instance!=nullptr){
        _server_instance->StartListeners();
    }
}
void MainWindow::StopListener(){
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
