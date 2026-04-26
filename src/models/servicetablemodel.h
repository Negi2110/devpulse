#ifndef SERVICETABLEMODEL_H
#define SERVICETABLEMODEL_H

#include <QAbstractTableModel>
#include "core/Service.h"
#include "core/Service.h"

class ServiceTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    // Column indices — enum so we never use magic numbers
    enum Column {
        ColName     = 0,
        ColUrl      = 1,
        ColStatus   = 2,
        ColLatency  = 3,
        ColInterval = 4,
        ColUptime   = 5,
        ColCount    = 6
    };

    explicit ServiceTableModel(QObject *parent = nullptr);

    // --- The 3 pure virtuals we MUST implement ---
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Column headers
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Our own methods to manage data
    void addService(const Service &service);
    void removeService(const QString &id);
    void updateService(const Service &service);
    QString serviceIdAt(int row) const;
private:
    QVector<Service> m_services;

    // Helper to find index by id
    int indexOfId(const QString &id) const;
};

#endif // SERVICETABLEMODEL_H



