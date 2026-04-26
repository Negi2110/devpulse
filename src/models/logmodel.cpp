#include "logmodel.h"
#include <QColor>

LogModel::LogModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int LogModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_entries.size();
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return {};
    if (index.row() >= m_entries.size()) return {};

    const LogEntry &e = m_entries[index.row()];

    if (role == Qt::DisplayRole) {
        return QString("[%1] %2: %3")
        .arg(e.timestamp.toLocalTime()
                 .toString("hh:mm:ss"))
            .arg(e.serviceName)
            .arg(e.message);
    }

    if (role == Qt::ForegroundRole) {
        switch (e.level) {
        case LogLevel::Error:   return QColor(Qt::red);
        case LogLevel::Warning: return QColor(255, 165, 0);
        default:                return QColor(Qt::lightGray);
        }
    }

    if (role == Qt::BackgroundRole)
        return QColor(30, 30, 30);

    return {};
}

void LogModel::addEntry(const LogEntry &entry)
{
    // Remove oldest if over limit
    if (m_entries.size() >= m_maxEntries) {
        beginRemoveRows(QModelIndex(), 0, 0);
        m_entries.removeFirst();
        endRemoveRows();
    }

    int row = m_entries.size();
    beginInsertRows(QModelIndex(), row, row);
    m_entries.append(entry);
    endInsertRows();
}

void LogModel::clearForService(const QString &serviceId)
{
    beginResetModel();
    m_entries.erase(
        std::remove_if(m_entries.begin(), m_entries.end(),
                       [&serviceId](const LogEntry &e) {
                           return e.serviceId == serviceId;
                       }),
        m_entries.end()
        );
    endResetModel();
}
