#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QVector>

enum class LogLevel {
    Info,
    Warning,
    Error
};

struct LogEntry {
    QDateTime timestamp;
    QString   serviceId;
    QString   serviceName;
    QString   message;
    LogLevel  level = LogLevel::Info;
};

class LogModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit LogModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;

    void addEntry(const LogEntry &entry);
    void clearForService(const QString &serviceId);
    void setMaxEntries(int max) { m_maxEntries = max; }

private:
    QVector<LogEntry> m_entries;
    int m_maxEntries = 500;
};

#endif // LOGMODEL_H
