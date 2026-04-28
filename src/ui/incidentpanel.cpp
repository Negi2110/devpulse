#include "incidentpanel.h"
#include "core/servicerepository.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>

IncidentPanel::IncidentPanel(ServiceRepository *repo,
                             DatabaseManager *db,
                             QWidget *parent)
    : QWidget(parent)
    , m_repo(repo)
    , m_tracker(new IncidentTracker(db))
{
    setupUi();
    refresh();
}

void IncidentPanel::setupUi()
{
    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({
        "Service", "Started", "Ended", "Duration"
    });
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::Fixed);
    m_table->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::Fixed);
    m_table->horizontalHeader()->setSectionResizeMode(
        2, QHeaderView::Fixed);
    m_table->setColumnWidth(0, 150);
    m_table->setColumnWidth(1, 180);
    m_table->setColumnWidth(2, 180);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->verticalHeader()->setVisible(false);
    m_table->setShowGrid(false);
    m_table->setAlternatingRowColors(true);

    m_refreshBtn = new QPushButton("Refresh", this);
    connect(m_refreshBtn, &QPushButton::clicked,
            this, &IncidentPanel::refresh);

    QHBoxLayout *topBar = new QHBoxLayout;
    topBar->addWidget(new QLabel("Incident History", this));
    topBar->addStretch();
    topBar->addWidget(m_refreshBtn);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(topBar);
    layout->addWidget(m_table);
}

void IncidentPanel::refresh()
{
    QVector<Service> services = m_repo->allServices();
    QVector<QPair<QString,QString>> pairs;
    for (const Service &s : services)
        pairs.append({s.id, s.name});

    QVector<Incident> incidents = m_tracker->allIncidents(pairs);
    populateTable(incidents);
}

void IncidentPanel::populateTable(const QVector<Incident> &incidents)
{
    m_table->setRowCount(0);

    for (const Incident &inc : incidents) {
        int row = m_table->rowCount();
        m_table->insertRow(row);

        m_table->setItem(row, 0,
                         new QTableWidgetItem(inc.serviceName));
        m_table->setItem(row, 1,
                         new QTableWidgetItem(
                             inc.startTime.toLocalTime()
                                 .toString("yyyy-MM-dd hh:mm:ss")));
        m_table->setItem(row, 2,
                         new QTableWidgetItem(
                             inc.isOngoing() ? "Ongoing" :
                                 inc.endTime.toLocalTime()
                                     .toString("yyyy-MM-dd hh:mm:ss")));

        QTableWidgetItem *durItem =
            new QTableWidgetItem(inc.durationString());

        // Color ongoing incidents red
        if (inc.isOngoing()) {
            durItem->setForeground(QColor("#f44747"));
            m_table->item(row, 0)
                ->setForeground(QColor("#f44747"));
        }

        m_table->setItem(row, 3, durItem);
    }

    if (incidents.isEmpty()) {
        m_table->insertRow(0);
        QTableWidgetItem *item =
            new QTableWidgetItem("No incidents recorded");
        item->setForeground(QColor("#858585"));
        m_table->setItem(0, 0, item);
        m_table->setSpan(0, 0, 1, 4);
    }
}
