#include "incidenttracker.h"
#include "core/storage/databasemanager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

IncidentTracker::IncidentTracker(DatabaseManager *db)
    : m_db(db)
{}

QVector<Incident> IncidentTracker::incidentsForService(
    const QString &serviceId,
    const QString &serviceName,
    int limit) const
{
    QVector<Incident> incidents;

    QSqlQuery q;
    q.prepare(
        "SELECT checked_at, status FROM check_history "
        "WHERE service_id = :sid "
        "ORDER BY id ASC"
        );
    q.bindValue(":sid", serviceId);

    if (!q.exec()) {
        qDebug() << "IncidentTracker query failed:" << q.lastError().text();
        return incidents;
    }

    Incident current;
    bool inIncident = false;

    while (q.next()) {
        QString checkedAt = q.value(0).toString();
        QString status    = q.value(1).toString();
        QDateTime dt      = QDateTime::fromString(checkedAt, Qt::ISODate);

        if (status == "Down" && !inIncident) {
            // Incident starts
            current = Incident();
            current.serviceId   = serviceId;
            current.serviceName = serviceName;
            current.startTime   = dt;
            current.durationSecs = -1;
            inIncident = true;

        } else if (status != "Down" && inIncident) {
            // Incident ends
            current.endTime     = dt;
            current.durationSecs = static_cast<int>(
                current.startTime.secsTo(dt));
            incidents.prepend(current);
            inIncident = false;

            if (incidents.size() >= limit) break;
        }
    }

    // Still ongoing
    if (inIncident) {
        current.endTime      = QDateTime();
        current.durationSecs = -1;
        incidents.prepend(current);
    }

    return incidents;
}

QVector<Incident> IncidentTracker::allIncidents(
    const QVector<QPair<QString,QString>> &services,
    int limit) const
{
    QVector<Incident> all;
    for (const auto &pair : services) {
        auto incidents = incidentsForService(pair.first, pair.second, limit);
        all += incidents;
    }

    // Sort by start time descending
    std::sort(all.begin(), all.end(),
              [](const Incident &a, const Incident &b) {
                  return a.startTime > b.startTime;
              });

    if (all.size() > limit)
        all.resize(limit);

    return all;
}
