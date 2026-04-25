

#include "servicetablemodel.h"
#include <QColor>

ServiceTableModel::ServiceTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int ServiceTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_services.size();
}

int ServiceTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return ColCount;
}

QVariant ServiceTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return {};
    if (index.row() >= m_services.size()) return {};

    const Service &s = m_services[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case ColName:     return s.name;
        case ColUrl:      return s.url;
        case ColStatus:   return s.statusString();
        case ColLatency:  return s.latencyMs < 0
                       ? QString("--")
                       : QString("%1 ms").arg(s.latencyMs);
        case ColInterval: return QString("%1 s").arg(s.intervalSecs);
        default:          return {};
        }
    }

    if (role == Qt::ForegroundRole && index.column() == ColStatus) {
        switch (s.status) {
        case ServiceStatus::Up:       return QColor(Qt::green);
        case ServiceStatus::Degraded: return QColor(255, 165, 0);
        case ServiceStatus::Down:     return QColor(Qt::red);
        default:                      return QColor(Qt::gray);
        }
    }

    return {};
}

QVariant ServiceTableModel::headerData(int section,
                                       Qt::Orientation orientation,
                                       int role) const
{
    if (role != Qt::DisplayRole) return {};
    if (orientation != Qt::Horizontal) return {};

    switch (section) {
    case ColName:     return "Name";
    case ColUrl:      return "URL / Address";
    case ColStatus:   return "Status";
    case ColLatency:  return "Latency";
    case ColInterval: return "Interval";
    default:          return {};
    }
}

void ServiceTableModel::addService(const Service &service)
{
    int row = m_services.size();
    beginInsertRows(QModelIndex(), row, row);
    m_services.append(service);
    endInsertRows();
}

void ServiceTableModel::removeService(const QString &id)
{
    int row = indexOfId(id);
    if (row < 0) return;
    beginRemoveRows(QModelIndex(), row, row);
    m_services.removeAt(row);
    endRemoveRows();
}

void ServiceTableModel::updateService(const Service &service)
{
    int row = indexOfId(service.id);
    if (row < 0) return;
    m_services[row] = service;
    QModelIndex topLeft     = index(row, 0);
    QModelIndex bottomRight = index(row, ColCount - 1);
    emit dataChanged(topLeft, bottomRight);
}

int ServiceTableModel::indexOfId(const QString &id) const
{
    for (int i = 0; i < m_services.size(); ++i) {
        if (m_services[i].id == id)
            return i;
    }
    return -1;
}
