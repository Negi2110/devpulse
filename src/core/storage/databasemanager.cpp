#include "databasemanager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDateTime>
#include <QDebug>

DatabaseManager::DatabaseManager()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

DatabaseManager::~DatabaseManager()
{
    close();
}

bool DatabaseManager::open(const QString &path)
{
    m_db.setDatabaseName(path);
    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        return false;
    }
    return createTables();
}

void DatabaseManager::close()
{
    if (m_db.isOpen())
        m_db.close();
}

bool DatabaseManager::isOpen() const
{
    return m_db.isOpen();
}

bool DatabaseManager::createTables()
{
    QSqlQuery q(m_db);

    bool ok = q.exec(
        "CREATE TABLE IF NOT EXISTS check_history ("
        "  id          INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  service_id  TEXT    NOT NULL,"
        "  checked_at  TEXT    NOT NULL,"
        "  status      TEXT    NOT NULL,"
        "  latency_ms  INTEGER NOT NULL"
        ")"
        );

    if (!ok) {
        m_lastError = q.lastError().text();
        qDebug() << "createTables failed:" << m_lastError;
        return false;
    }

    // Index for fast queries by service
    q.exec(
        "CREATE INDEX IF NOT EXISTS idx_service_id "
        "ON check_history(service_id)"
        );

    return true;
}

bool DatabaseManager::insertCheckResult(const QString &serviceId,
                                        const QString &status,
                                        int latencyMs)
{
    QSqlQuery q(m_db);
    q.prepare(
        "INSERT INTO check_history "
        "(service_id, checked_at, status, latency_ms) "
        "VALUES (:sid, :at, :status, :latency)"
        );

    q.bindValue(":sid",     serviceId);
    q.bindValue(":at",      QDateTime::currentDateTimeUtc()
                           .toString(Qt::ISODate));
    q.bindValue(":status",  status);
    q.bindValue(":latency", latencyMs);

    if (!q.exec()) {
        m_lastError = q.lastError().text();
        return false;
    }
    return true;
}

QVector<CheckRecord> DatabaseManager::recentHistory(
    const QString &serviceId, int limit) const
{
    QVector<CheckRecord> records;

    QSqlQuery q(m_db);
    q.prepare(
        "SELECT service_id, checked_at, status, latency_ms "
        "FROM check_history "
        "WHERE service_id = :sid "
        "ORDER BY id DESC "
        "LIMIT :limit"
        );
    q.bindValue(":sid",   serviceId);
    q.bindValue(":limit", limit);

    if (!q.exec()) {
        qDebug() << "recentHistory failed:" << q.lastError().text();
        return records;
    }

    while (q.next()) {
        CheckRecord r;
        r.serviceId = q.value(0).toString();
        r.checkedAt = q.value(1).toString();
        r.status    = q.value(2).toString();
        r.latencyMs = q.value(3).toInt();
        records.append(r);
    }

    return records;
}

double DatabaseManager::uptimePercent(
    const QString &serviceId, int lastNChecks) const
{
    QSqlQuery q(m_db);
    q.prepare(
        "SELECT COUNT(*) FROM ("
        "  SELECT status FROM check_history "
        "  WHERE service_id = :sid "
        "  ORDER BY id DESC LIMIT :n"
        ")"
        );
    q.bindValue(":sid", serviceId);
    q.bindValue(":n",   lastNChecks);
    q.exec();
    q.next();
    int total = q.value(0).toInt();
    if (total == 0) return 100.0;

    q.prepare(
        "SELECT COUNT(*) FROM ("
        "  SELECT status FROM check_history "
        "  WHERE service_id = :sid "
        "  AND status = 'Up' "
        "  ORDER BY id DESC LIMIT :n"
        ")"
        );
    q.bindValue(":sid", serviceId);
    q.bindValue(":n",   lastNChecks);
    q.exec();
    q.next();
    int upCount = q.value(0).toInt();

    return (static_cast<double>(upCount) / total) * 100.0;
}
