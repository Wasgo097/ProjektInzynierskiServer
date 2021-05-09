#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serverinstance.h"
#include "Global.h"
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    _server_instance=ServerInstance::GetInstance(this);
    StartListeners();
    StarManager();
}
MainWindow::~MainWindow(){
    StopManager();
    StopListeners();
    delete ui;
}
void MainWindow::StartListeners(){
    if(_server_instance!=nullptr){
        _server_instance->StartListeners();
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

}
