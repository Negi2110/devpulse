#ifndef INCIDENTPANEL_H
#define INCIDENTPANEL_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include "core/monitoring/incidenttracker.h"

class ServiceRepository;
class DatabaseManager;

class IncidentPanel : public QWidget
{
    Q_OBJECT

public:
    explicit IncidentPanel(ServiceRepository *repo,
                           DatabaseManager *db,
                           QWidget *parent = nullptr);

public slots:
    void refresh();

private:
    void setupUi();
    void populateTable(const QVector<Incident> &incidents);

    QTableWidget      *m_table;
    QPushButton       *m_refreshBtn;
    ServiceRepository *m_repo;
    IncidentTracker   *m_tracker;
};

#endif // INCIDENTPANEL_H
