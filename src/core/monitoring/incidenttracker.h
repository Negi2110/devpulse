#ifndef INCIDENTTRACKER_H
#define INCIDENTTRACKER_H

#include <QString>
#include <QVector>
#include <QDateTime>

struct Incident {
    QString   serviceId;
    QString   serviceName;
    QDateTime startTime;
    QDateTime endTime;      // invalid if still ongoing
    int       durationSecs; // -1 if still ongoing

    bool isOngoing() const { return !endTime.isValid(); }

    QString durationString() const {
        if (isOngoing()) return "Ongoing";
        if (durationSecs < 60)
            return QString("%1s").arg(durationSecs);
        if (durationSecs < 3600)
            return QString("%1m %2s")
                .arg(durationSecs / 60)
                .arg(durationSecs % 60);
        return QString("%1h %2m")
            .arg(durationSecs / 3600)
            .arg((durationSecs % 3600) / 60);
    }
};

class DatabaseManager;

class IncidentTracker
{
public:
    explicit IncidentTracker(DatabaseManager *db);

    // Get all incidents for a service
    QVector<Incident> incidentsForService(
        const QString &serviceId,
        const QString &serviceName,
        int limit = 50) const;

    // Get all incidents across all services
    QVector<Incident> allIncidents(
        const QVector<QPair<QString,QString>> &services,
        int limit = 100) const;

private:
    DatabaseManager *m_db;
};

#endif // INCIDENTTRACKER_H
