#include "servicetablemodel.h"
#include <QColor>
#include <QFont>

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
        case ColUptime:
            return QString("%1%").arg(s.uptimePercent, 0, 'f', 1);
        default:          return {};
        }
    }

    if (role == Qt::ForegroundRole && index.column() == ColStatus) {
        switch (s.status) {
        case ServiceStatus::Up:
            return QColor("#4ec9b0");      // teal green
        case ServiceStatus::Degraded:
            return QColor("#ce9178");      // orange
        case ServiceStatus::Down:
            return QColor("#f44747");      // bright red
        default:
            return QColor("#858585");      // gray
        }
    }

    if (role == Qt::FontRole && index.column() == ColStatus) {
        QFont font;
        font.setBold(true);
        return font;
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case ColLatency:
        case ColUptime:
            return Qt::AlignRight + Qt::AlignVCenter;
        case ColStatus:
            return Qt::AlignCenter;
        default:
            return Qt::AlignLeft + Qt::AlignVCenter;
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
    case ColUptime:   return "Uptime";
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
QString ServiceTableModel::serviceIdAt(int row) const
{
    if (row < 0 || row >= m_services.size()) return {};
    return m_services[row].id;
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
