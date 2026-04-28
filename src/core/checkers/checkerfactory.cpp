#include "checkerfactory.h"
#include "core/checkers/httpchecker.h"
#include "core/checkers/tcpchecker.h"
#include "core/plugins/PluginLoader.h"
#include "core/plugins/ICheckerPlugin.h"
#include <QUrl>
#include <QDebug>

PluginLoader* CheckerFactory::s_pluginLoader = nullptr;

void CheckerFactory::setPluginLoader(PluginLoader *loader)
{
    s_pluginLoader = loader;
}

IChecker* CheckerFactory::create(ServiceType type, QObject *parent)
{
    switch (type) {
    case ServiceType::Http:  return new HttpChecker(parent);
    case ServiceType::Tcp:   return new TcpChecker(parent);
    default:                 return new HttpChecker(parent);
    }
}

IChecker* CheckerFactory::create(const Service &service, QObject *parent)
{
    if (s_pluginLoader) {
        QString scheme = QUrl(service.url).scheme();
        ICheckerPlugin *plugin = s_pluginLoader->pluginForScheme(scheme);
        if (plugin) {
            qDebug() << "Plugin:" << plugin->name() << "for" << service.name;
            return plugin->createChecker(parent);
        }
    }
    return create(service.type, parent);
}
