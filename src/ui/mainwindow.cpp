#include "ui/mainwindow.h"
#include <QApplication>
#include <QMenu>
#include <QAction>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi()
{
    // Window properties
    setWindowTitle("DevPulse");
    setMinimumSize(1100, 650);

    // Status bar
    m_statusLabel = new QLabel("All systems operational", this);
    statusBar()->addWidget(m_statusLabel);

    // File menu
    QMenu *fileMenu = menuBar()->addMenu("&File");

    QAction *quitAction = new QAction("&Quit", this);
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered,
            this, &MainWindow::onFileQuit);

    fileMenu->addAction(quitAction);

    // Help menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    QAction *aboutAction = new QAction("&About DevPulse", this);
    helpMenu->addAction(aboutAction);
}

void MainWindow::onFileQuit()
{
    QApplication::quit();
}
