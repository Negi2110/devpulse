#ifndef ICHECKERPLUGIN_H
#define ICHECKERPLUGIN_H

#include <QtPlugin>
#include "core/checkers/IChecker.h"

#define ICheckerPlugin_IID "com.devpulse.ICheckerPlugin/1.0"

class ICheckerPlugin
{
public:
    virtual ~ICheckerPlugin() = default;
    virtual QString name() const = 0;
    virtual QString version() const = 0;
    virtual QString description() const = 0;
    virtual QStringList supportedSchemes() const = 0;
    virtual IChecker* createChecker(QObject *parent = nullptr) = 0;
};

Q_DECLARE_INTERFACE(ICheckerPlugin, ICheckerPlugin_IID)

#endif // ICHECKERPLUGIN_H
