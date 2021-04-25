#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QDebug>
#include <memory>
class ServerInstance;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui=nullptr;
    ServerInstance *_server_instance=nullptr;
private:
    void StartListener();
    void StopListener();
    void StarManager();
    void StopManager();
};
#endif // MAINWINDOW_H
