#ifndef JSONPROFILEMANAGER_H
#define JSONPROFILEMANAGER_H

#include <QString>
#include <QVector>
#include "core/Service.h"

class JsonProfileManager
{
public:
    JsonProfileManager() = default;

    // Save all services to a .json file
    // Returns true on success
    bool save(const QVector<Service> &services,
              const QString &filePath);

    // Load services from a .json file
    // Returns empty vector on failure
    QVector<Service> load(const QString &filePath);

    // Last error message if save/load failed
    QString lastError() const { return m_lastError; }

private:
    QJsonObject serviceToJson(const Service &service);
    Service     serviceFromJson(const QJsonObject &obj);

    QString m_lastError;
};

#endif // JSONPROFILEMANAGER_H
