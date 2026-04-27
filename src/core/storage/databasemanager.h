#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QVector>
#include <QtSql/QSqlDatabase>
#include "core/Service.h"

struct CheckRecord {
    QString serviceId;
    QString checkedAt;
    QString status;
    int     latencyMs;
};

class DatabaseManager
{
public:
    DatabaseManager();
    ~DatabaseManager();

    bool open(const QString &path);
    void close();
    bool isOpen() const;

    // Schema
    bool createTables();

    // Check history
    bool insertCheckResult(const QString &serviceId,
                           const QString &status,
                           int latencyMs);

    QVector<CheckRecord> recentHistory(const QString &serviceId,
                                       int limit = 100) const;

    // Uptime from DB
    double uptimePercent(const QString &serviceId,
                         int lastNChecks = 100) const;

    QString lastError() const { return m_lastError; }

private:
    QSqlDatabase m_db;
    QString      m_lastError;
};

#endif // DATABASEMANAGER_H
