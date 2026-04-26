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
#include "ui/latencygraphwidget.h"
#include <QSplitter>

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

    // Create graph widget
    m_graphWidget = new LatencyGraphWidget(this);
    m_graphWidget->setMinimumHeight(250);

    // Splitter — table on top, graph on bottom
    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(m_tableView);
    splitter->addWidget(m_graphWidget);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);

    setCentralWidget(splitter);
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

    connect(m_engine, &MonitorEngine::serviceStatusChanged,
            this, [this](const Service &service){
               m_graphWidget->updateForService(
                  service.id,
                m_engine->latencyStore()
            );
    });

    connect(m_tableView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            this, [this](const QModelIndex &current, const QModelIndex&) {
                if (!current.isValid()) return;
                QString id = m_tableModel->serviceIdAt(current.row());
                m_graphWidget->updateForService(id, m_engine->latencyStore());
                m_graphWidget->setTitle(
                    m_tableModel->data(
                                    m_tableModel->index(current.row(), 0)
                                    ).toString() + " — Latency"
                    );
            });
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
