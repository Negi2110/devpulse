#include "jsonprofilemanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

bool JsonProfileManager::save(const QVector<Service> &services,
                              const QString &filePath)
{
    QJsonArray array;
    for (const Service &s : services)
        array.append(serviceToJson(s));

    QJsonObject root;
    root["version"]  = 1;
    root["services"] = array;

    QJsonDocument doc(root);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "Cannot open file for writing: " + filePath;
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

QVector<Service> JsonProfileManager::load(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = "Cannot open file: " + filePath;
        return {};
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (doc.isNull()) {
        m_lastError = "JSON parse error: " + error.errorString();
        return {};
    }

    QJsonObject root     = doc.object();
    QJsonArray  array    = root["services"].toArray();

    QVector<Service> services;
    for (const QJsonValue &val : array) {
        if (val.isObject())
            services.append(serviceFromJson(val.toObject()));
    }

    return services;
}

QJsonObject JsonProfileManager::serviceToJson(const Service &service)
{
    QJsonObject obj;
    obj["id"]                   = service.id;
    obj["name"]                 = service.name;
    obj["url"]                  = service.url;
    obj["type"]                 = (service.type == ServiceType::Http)
                      ? "http" : "tcp";
    obj["intervalSecs"]         = service.intervalSecs;
    obj["timeoutMs"]            = service.timeoutMs;
    obj["degradedThresholdMs"]  = service.degradedThresholdMs;
    obj["notes"]                = service.notes;
    return obj;
}

Service JsonProfileManager::serviceFromJson(const QJsonObject &obj)
{
    Service s;
    s.id                   = obj["id"].toString();
    s.name                 = obj["name"].toString();
    s.url                  = obj["url"].toString();
    s.type                 = (obj["type"].toString() == "http")
                 ? ServiceType::Http : ServiceType::Tcp;
    s.intervalSecs         = obj["intervalSecs"].toInt(30);
    s.timeoutMs            = obj["timeoutMs"].toInt(5000);
    s.degradedThresholdMs  = obj["degradedThresholdMs"].toInt(2000);
    s.notes                = obj["notes"].toString();
    s.addedAt              = QDateTime::currentDateTimeUtc();
    return s;
}
