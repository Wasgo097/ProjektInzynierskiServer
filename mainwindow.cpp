#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serverinstance.h"
#include "Global.h"
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    _serverInstance=ServerInstance::GetInstance(this);
    StartListening();
}
MainWindow::~MainWindow(){
    StopListening();
    delete ui;
}
void MainWindow::StartListening(){
    if(_serverInstance!=nullptr)
        _serverInstance->StartListening();
}
void MainWindow::StopListening(){
    if(_serverInstance!=nullptr)
        _serverInstance->StopListening();
}
