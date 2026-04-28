#include "PluginLoader.h"
#include <QDir>
#include <QPluginLoader>
#include <QDebug>

PluginLoader::PluginLoader(QObject *parent)
    : QObject(parent)
{}

int PluginLoader::loadPlugins(const QString &directory)
{
    QDir dir(directory);
    if (!dir.exists()) {
        qDebug() << "Plugin dir not found:" << directory;
        return 0;
    }

    QStringList filters;
    filters << "*.so" << "*.dll" << "*.dylib";
    dir.setNameFilters(filters);

    for (const QString &fileName : dir.entryList(QDir::Files))
        loadPlugin(dir.absoluteFilePath(fileName));

    qDebug() << "Loaded" << m_plugins.size() << "plugins";
    return m_plugins.size();
}

void PluginLoader::loadPlugin(const QString &filePath)
{
    QPluginLoader loader(filePath);
    QObject *obj = loader.instance();

    if (!obj) {
        qDebug() << "Failed:" << filePath << loader.errorString();
        return;
    }

    ICheckerPlugin *plugin = qobject_cast<ICheckerPlugin*>(obj);
    if (!plugin) {
        qDebug() << "Not valid ICheckerPlugin:" << filePath;
        loader.unload();
        return;
    }

    m_plugins.append(plugin);
    for (const QString &scheme : plugin->supportedSchemes())
        m_schemeMap[scheme] = plugin;

    qDebug() << "Plugin loaded:" << plugin->name();
}

ICheckerPlugin* PluginLoader::pluginForScheme(const QString &scheme) const
{
    return m_schemeMap.value(scheme, nullptr);
}

QVector<ICheckerPlugin*> PluginLoader::plugins() const
{
    return m_plugins;
}
