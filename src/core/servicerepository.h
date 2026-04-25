#ifndef SERVICEREPOSITORY_H
#define SERVICEREPOSITORY_H

#include <QObject>
#include <QVector>
#include "Service.h"

class ServiceRepository : public QObject
{
    Q_OBJECT

public:
    explicit ServiceRepository(QObject *parent = nullptr);

    // Add a new service — emits serviceAdded
    void addService(const Service &service);

    // Remove by ID — emits serviceRemoved
    void removeService(const QString &id);

    // Update existing — emits serviceUpdated
    void updateService(const Service &service);

    // Queries
    Service findById(const QString &id) const;
    QVector<Service> allServices() const;
    int count() const;

signals:
    void serviceAdded(const Service &service);
    void serviceRemoved(const QString &id);
    void serviceUpdated(const Service &service);

private:
    QVector<Service> m_services;
};

#endif // SERVICEREPOSITORY_H
