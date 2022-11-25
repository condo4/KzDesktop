#include "applicationmanager.h"
#include "waylandentries.h"
#include <QWaylandXdgSurface>
#include <QWaylandXdgToplevel>

ApplicationManager::ApplicationManager(QObject *parent)
    : QObject(parent)
{

}

bool ApplicationManager::isHome() const
{
    return m_isHome;
}

QObject *ApplicationManager::currentSurface() const
{
    return m_currentSurface;
}

void ApplicationManager::setCurrentSurface(QObject *newCurrentSurface)
{
    if (m_currentSurface == newCurrentSurface)
    {
        emit currentSurfaceChanged();
        return;
    }
    m_currentSurface = newCurrentSurface;
    emit currentSurfaceChanged();
}

int ApplicationManager::count()
{
    WaylandEntries *entries = WaylandEntries::getInstance();
    if(!entries)
        return 0;
    return entries->rowCount();
}

QString ApplicationManager::applicationName(int id)
{
    WaylandEntries *entries = WaylandEntries::getInstance();
    if(!entries)
        return "";
    return entries->data(entries->index(id), WaylandEntries::TitleRole).toString();
}

int ApplicationManager::applicationPid(int id)
{
    WaylandEntries *entries = WaylandEntries::getInstance();
    if(!entries)
        return 0;
    return entries->data(entries->index(id), WaylandEntries::PidRole).toInt();
}

int ApplicationManager::applicationFocus(int id)
{
    WaylandEntries *entries = WaylandEntries::getInstance();
    if(!entries)
        return -1;
    if(id >= entries->rowCount())
        return -2;
    QVariant d = entries->data(entries->index(id), WaylandEntries::SurfaceRole);
    if(!d.isValid())
        return -3;
    QWaylandXdgSurface *surface = qvariant_cast<QWaylandXdgSurface *>(d);
    if(surface)
    {
        setCurrentSurface(surface);
        return 0;
    }
    return -4;
}

int ApplicationManager::currentApplicationPid()
{
    if(isHome())
        return -1;
    WaylandEntries *entries = WaylandEntries::getInstance();
    for(int i = 0; i < entries->rowCount(); i++)
    {
        QVariant d = entries->data(entries->index(i), WaylandEntries::SurfaceRole);
        QWaylandXdgSurface *surface = qvariant_cast<QWaylandXdgSurface *>(d);
        QWaylandXdgSurface *current = qobject_cast<QWaylandXdgSurface*>(currentSurface());
        if(surface != current)
            continue;
        return entries->data(entries->index(i), WaylandEntries::PidRole).toInt();
    }
    return -2;
}

void ApplicationManager::hideApplicationPid(qint64 pid)
{
    WaylandEntries *entries = WaylandEntries::getInstance();
    entries->addHidePid(pid);
}

void ApplicationManager::raiseApplicationPid(qint64 pid)
{
    WaylandEntries *entries = WaylandEntries::getInstance();
    QObject *d = entries->getSurface(pid);
    QWaylandXdgSurface *surface = qobject_cast<QWaylandXdgSurface *>(d);
    if(surface)
    {
        setCurrentSurface(surface);
    }
}

void ApplicationManager::backHome()
{
    emit gotoHome();
}

void ApplicationManager::setIsHome(bool isHome)
{
    if (m_isHome == isHome)
        return;

    m_isHome = isHome;
    emit isHomeChanged(m_isHome);
}

QString ApplicationManager::virtualkeyboard()
{
    return qgetenv("QT_IM_MODULE");
}
