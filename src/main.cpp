#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDBusConnection>
#include <QIcon>
#include <QTimer>
#include <QThread>
#include <systemd/sd-daemon.h>
#include <QDir>
#include <QFileInfo>
#include <xdgstatusnotifierwatcher.h>
#include "applicationmanager.h"
#include "applicationsadaptor.h"
#include "waylandentries.h"
#include "waylandentriesfiltered.h"

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", "qtvirtualkeyboard");
    QDBusConnection bus = QDBusConnection::sessionBus();
    QGuiApplication app(argc, argv);
    ApplicationManager manager;
    QTimer watchdog;
    uint64_t watchdogPeriodUSec;

    QFileInfo volatilapps("/var/volatile/applications");
    if(!volatilapps.exists())
    {
        QDir().mkdir("/var/volatile/applications");
    }

    qmlRegisterType<WaylandEntries>("org.kazoe.desktop", 1, 0, "WaylandEntries");
    qmlRegisterType<WaylandEntriesFiltered>("org.kazoe.desktop", 1, 0, "WaylandEntriesFiltered");

    if (!bus.isConnected()) {
        qWarning("Cannot connect to the D-Bus session bus.\n"
                 "Please check your system settings and try again.\n");
        return 1;
    }

    QQmlApplicationEngine engine;
    new ApplicationsAdaptor(&manager);
    bus.registerObject("/applications", &manager);


    bool conres = bus.registerService("org.kazoe.desktop");
    if(!conres) qWarning() << "Can't register dbus service org.kazoe.desktop";

    KaZoe::StatusNotifierWatcher notifierWatcher;

    engine.rootContext()->setContextProperty("manager", &manager);
    engine.loadFromModule("org.kazoe.desktop", "Main");

    // signal systemd that the app the ready
    QTimer::singleShot(0, [] () {
        sd_notify(0, "READY=1");
    }
    );

    // setup a timer to refresh the systemd'd watchdog if needed
    if (sd_watchdog_enabled(0,&watchdogPeriodUSec) > 0) {
        QObject::connect(&watchdog, &QTimer::timeout, [] () {
            sd_notify(0, "WATCHDOG=1");
        }
        );
    }
    // configure the period of the QT timer for a half period of the watchdog timer
    watchdog.start(watchdogPeriodUSec / 2000);
    return app.exec();
}
