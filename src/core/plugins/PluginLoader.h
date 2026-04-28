#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>
#include <QVector>
#include <QMap>
#include "ICheckerPlugin.h"

class PluginLoader : public QObject
{
    Q_OBJECT

public:
    explicit PluginLoader(QObject *parent = nullptr);

    int loadPlugins(const QString &directory);
    ICheckerPlugin* pluginForScheme(const QString &scheme) const;
    QVector<ICheckerPlugin*> plugins() const;
    int count() const { return m_plugins.size(); }

private:
    void loadPlugin(const QString &filePath);

    QVector<ICheckerPlugin*>       m_plugins;
    QMap<QString, ICheckerPlugin*> m_schemeMap;
};

#endif // PLUGINLOADER_H
