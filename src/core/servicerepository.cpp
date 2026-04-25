#include "servicerepository.h"

ServiceRepository::ServiceRepository(QObject *parent)
    : QObject(parent)
{}

void ServiceRepository::addService(const Service &service)
{
    for (const Service &s : m_services) {
        if (s.id == service.id)
            return;
    }
    m_services.append(service);
    emit serviceAdded(service);
}

void ServiceRepository::removeService(const QString &id)
{
    for (int i = 0; i < m_services.size(); ++i) {
        if (m_services[i].id == id) {
            m_services.removeAt(i);
            emit serviceRemoved(id);
            return;
        }
    }
}

void ServiceRepository::updateService(const Service &service)
{
    for (int i = 0; i < m_services.size(); ++i) {
        if (m_services[i].id == service.id) {
            m_services[i] = service;
            emit serviceUpdated(service);
            return;
        }
    }
}

Service ServiceRepository::findById(const QString &id) const
{
    for (const Service &s : m_services) {
        if (s.id == id)
            return s;
    }
    return Service{};
}

QVector<Service> ServiceRepository::allServices() const
{
    return m_services;
}

int ServiceRepository::count() const
{
    return m_services.size();
}
