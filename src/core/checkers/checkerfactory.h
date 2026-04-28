#ifndef CHECKERFACTORY_H
#define CHECKERFACTORY_H

#include "core/checkers/IChecker.h"
#include "core/Service.h"

class PluginLoader;

class CheckerFactory
{
public:
    static IChecker* create(ServiceType type, QObject *parent = nullptr);
    static IChecker* create(const Service &service, QObject *parent = nullptr);
    static void setPluginLoader(PluginLoader *loader);

private:
    CheckerFactory() = delete;
    static PluginLoader *s_pluginLoader;
};

#endif // CHECKERFACTORY_H
