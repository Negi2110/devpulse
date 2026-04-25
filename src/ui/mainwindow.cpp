#include "ui/mainwindow.h"
#include "core/servicerepository.h"
#include "models/servicetablemodel.h"
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QWidget>
#include <QHeaderView>
#include <QStatusBar>
#include <QMenuBar>
#include "ui/dialogs/addservicedialog.h"
#include "core/monitorengine.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_repo(new ServiceRepository(this))
    , m_tableModel(new ServiceTableModel(this))
    , m_engine(new MonitorEngine(m_repo, this))

{
    setupUi();
    connectSignals();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi()
{
    setWindowTitle("DevPulse");
    setMinimumSize(1100, 650);

    // Table view
    m_tableView = new QTableView(this);
    m_tableView->setModel(m_tableModel);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->setAlternatingRowColors(true);

    setCentralWidget(m_tableView);

    // Status bar
    m_statusLabel = new QLabel("No services added", this);
    statusBar()->addWidget(m_statusLabel);

    // Menu
    QMenu *fileMenu = menuBar()->addMenu("&File");

    QAction *addAction = new QAction("&Add Service", this);
    addAction->setShortcut(QKeySequence("Ctrl+N"));
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddService);
    fileMenu->addAction(addAction);

    fileMenu->addSeparator();

    QAction *quitAction = new QAction("&Quit", this);
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, this, &MainWindow::onFileQuit);
    fileMenu->addAction(quitAction);
}

void MainWindow::connectSignals()
{
    connect(m_repo, &ServiceRepository::serviceAdded,
            m_tableModel, &ServiceTableModel::addService);

    connect(m_repo, &ServiceRepository::serviceRemoved,
            m_tableModel, &ServiceTableModel::removeService);

    connect(m_repo, &ServiceRepository::serviceUpdated,
            m_tableModel, &ServiceTableModel::updateService);
}

void MainWindow::onFileQuit()
{
    QApplication::quit();
}

void MainWindow::onAddService()
{
    AddServiceDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        m_repo->addService(dlg.service());
        m_statusLabel->setText(
            QString("%1 service(s) monitored").arg(m_repo->count())
            );
    }
}
