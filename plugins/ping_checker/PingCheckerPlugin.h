#ifndef PINGCHECKERPLUGIN_H
#define PINGCHECKERPLUGIN_H

#include <QObject>
#include "core/plugins/ICheckerPlugin.h"
#include "core/checkers/httpchecker.h"

class PingCheckerPlugin : public QObject, public ICheckerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ICheckerPlugin_IID)
    Q_INTERFACES(ICheckerPlugin)

public:
    QString name() const override { return "PingChecker"; }
    QString version() const override { return "1.0.0"; }
    QString description() const override {
        return "HTTP ping checker as plugin demo";
    }
    QStringList supportedSchemes() const override {
        return {"ping"};
    }
    IChecker* createChecker(QObject *parent = nullptr) override {
        return new HttpChecker(parent);
    }
};

#endif // PINGCHECKERPLUGIN_H
