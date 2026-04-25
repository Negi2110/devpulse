#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include "core/servicerepository.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onFileQuit();
    void onAddService();

private:
    void setupUi();

    QLabel *m_statusLabel;
    ServiceRepository   *m_repo;

};

#endif // MAINWINDOW_H
