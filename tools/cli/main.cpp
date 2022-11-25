#include <QCoreApplication>
#include <QDBusConnection>
#include <QTimer>
#include "ApplicationManagerIface.h"

static int query(OrgKazoeDesktopApplicationsInterface &iface, int argc, char *argv[])
{
    if(argc < 2 || QString(argv[1]) == "list")
    {
        int current = iface.currentApplicationPid().value();
        printf(" ______________________________________________________\n");
        printf("| ID     | Name                             | PID      |\n");
        printf("|--------|----------------------------------|----------|\n");
        if(current == -1)
        {
            printf("|> HOME  | Homepage                         |          |\n");
        }
        else
        {
            printf("|  HOME  | Homepage                         |          |\n");
        }

        for(int i = 0; i < iface.count(); i++)
        {
            int pid = iface.applicationPid(i).value();
            if(current == pid)
            {
                printf("|> @%04i | ", i);
            }
            else
            {
                printf("|  @%04i | ", i);
            }
            printf("%-32s |", QString(iface.applicationName(i)).mid(0,32).toStdString().c_str());
            if(pid) printf("% 9i | ", iface.applicationPid(i).value());
            else printf("          |");
            printf("\n");
        }
        printf("--------------------------------------------------------\n");
        return 0;
    }

    if(QString(argv[1]) == "focus" && argc == 3)
    {
        int id = -1;
        QString ids = QString(argv[2]);
        if(ids.startsWith("@"))
        {
            id = ids.mid(1).toInt();
            return iface.applicationFocus(id).value();
        }
        for(int i = 0; i < iface.count(); i++)
        {
            QString name = iface.applicationName(i);
            if(name == ids)
            {
                return iface.applicationFocus(i).value();
            }
        }
        return -1;
    }

    if(QString(argv[1]) == "home")
    {
        iface.backHome();
        return 0;
    }

    return -1;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.isConnected()) {
        qWarning("Cannot connect to the D-Bus session bus.\n"
                 "Please check your system settings and try again.\n");
        return 1;
    }
    const QString service = "org.kazoe.desktop.applications";
    const QString path = "/applications";
    OrgKazoeDesktopApplicationsInterface iface(service, path, connection);

    QTimer::singleShot(0, [argc, argv, &iface](){
        if(query(iface, argc, argv) != 0)
        {
            qInfo() << "Usage:";
            qInfo() << "> desktopcli list              -> List all views";
            qInfo() << "> desktopcli home              -> Go to home view";
            qInfo() << "> desktopcli focus <ID | Name> -> Go to application view, by ID or Name";
        }
        exit(0);
    });

    return app.exec();
}
