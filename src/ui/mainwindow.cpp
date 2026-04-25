#include "ui/mainwindow.h"
#include <QApplication>
#include <QMenu>
#include <QAction>
#include "ui/dialogs/addservicedialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
     , m_repo(new ServiceRepository(this))
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

    QAction *addAction = new QAction("&Add Service", this);
    addAction->setShortcut(QKeySequence("Ctrl+N"));
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddService);
    fileMenu->addAction(addAction);

    // Help menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    QAction *aboutAction = new QAction("&About DevPulse", this);
    helpMenu->addAction(aboutAction);
}

void MainWindow::onAddService()
{
    AddServiceDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        m_repo->addService(dlg.service());
        m_statusLabel->setText(
            QString("Services: %1").arg(m_repo->count())
            );
    }
}
void MainWindow::onFileQuit()
{
    QApplication::quit();
}
