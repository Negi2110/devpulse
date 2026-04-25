#include "ui/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("DevPulse");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("devpulse");

    MainWindow w;
    w.show();

    return app.exec();
}
