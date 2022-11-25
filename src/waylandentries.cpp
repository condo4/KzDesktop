#include "waylandentries.h"
#include <QWaylandXdgSurface>
#include <QWaylandSurface>
#include <QMutexLocker>
#include <qdebug.h>

WaylandEntries *m_instance = nullptr;

WaylandEntries::WaylandEntries(QObject *parent)
    : QAbstractListModel(parent)
{
    if(m_instance != nullptr)
    {
        qWarning() << "Many instance of WaylandEntries";
    }
    m_instance = this;
}

int WaylandEntries::rowCount(const QModelIndex &parent) const
{
    QMutexLocker locker(&m_lock);
    return m_surfaces.count();
}

QVariant WaylandEntries::data(const QModelIndex &index, int role) const
{
    QMutexLocker locker(&m_lock);
    if(index.row() > m_surfaces.count())
        return QVariant();

    QWaylandXdgSurface *surface = m_surfaces[index.row()];
    if(surface == nullptr)
        return QVariant();

    switch(role) {
        case Qt::DisplayRole:
        case TitleRole:
            return surface->toplevel()->title();
        case AppIdRole:
            return surface->toplevel()->appId();
        case SurfaceRole:
            return QVariant::fromValue<QWaylandXdgSurface*>(surface);
        case PidRole:
            return surface->surface()->client()->processId();
    }

    return QVariant();
}

QHash<int, QByteArray> WaylandEntries::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();
    roles[SurfaceRole] = "wl_surface";
    roles[TitleRole] = "title";
    roles[PidRole] = "pid";
    roles[AppIdRole] = "appId";
    return roles;
}

// Need only for application manager interface for cli
WaylandEntries *WaylandEntries::getInstance()
{
    return m_instance;
}

void WaylandEntries::addSurface(QObject *obj)
{
    QMutexLocker locker(&m_lock);
    QWaylandXdgSurface *s = qobject_cast<QWaylandXdgSurface*>(obj);
    if(s)
    {
        QObject::connect(s, &QWaylandXdgSurface::destroyed, this, &WaylandEntries::surfaceDestroyed);
        if(m_hidden.contains(s->surface()->client()->processId()))
        {
            m_surfaces_hidden.append(s);
        }
        else
        {
            int id = m_surfaces.count();
            beginInsertRows(QModelIndex(), m_surfaces.count(), m_surfaces.count());
            QObject::connect(s->toplevel(), &QWaylandXdgToplevel::titleChanged, this, [this, id](){
                const QVector<int> roles = {TitleRole};
                emit dataChanged(index(id), index(id), roles);
            });
            QObject::connect(s->toplevel(), &QWaylandXdgToplevel::appIdChanged, this, [this, id, s](){
                const QVector<int> roles = {AppIdRole};

                emit topWindowRegistered(s, s->toplevel()->appId());
                emit dataChanged(index(id), index(id), roles);
            });
            m_surfaces.append(s);
            endInsertRows();
            emit nbAppsChanged();
        }
    }
}

void WaylandEntries::addHidePid(qint64 pid)
{
    QMutexLocker locker(&m_lock);
    m_hidden.append(pid);
    QWaylandXdgSurface *surfaceToHide = nullptr;

    for(QWaylandXdgSurface *surface: m_surfaces)
    {
        if(surface->surface()->client()->processId() == pid)
        {
            surfaceToHide = surface;
            break;
        }
    }
    if(surfaceToHide)
    {
        beginRemoveRows(QModelIndex(), m_surfaces.indexOf(surfaceToHide), m_surfaces.indexOf(surfaceToHide));
        m_surfaces.removeAll(surfaceToHide);
        endRemoveRows();
        m_surfaces_hidden.append(surfaceToHide);
        emit nbAppsChanged();
    }
}

void WaylandEntries::surfaceDestroyed()
{
    QObject *obj = QObject::sender();
    QWaylandXdgSurface *s = static_cast<QWaylandXdgSurface*>(obj);
    emit topWindowDestroyed(s, s->toplevel()->appId());
    QMutexLocker locker(&m_lock);
    beginRemoveRows(QModelIndex(), m_surfaces.indexOf(s), m_surfaces.indexOf(s));
    m_surfaces.removeAll(s);
    endRemoveRows();
    emit nbAppsChanged();
}

void WaylandEntries::stop(int pid)
{
    kill(pid, SIGTERM);
}

QObject *WaylandEntries::getSurface(int pid)
{
    QMutexLocker locker(&m_lock);
    for(QWaylandXdgSurface *surface: m_surfaces)
    {
        if(surface->surface()->client()->processId() == pid)
            return surface;
    }
    for(QWaylandXdgSurface *surface: m_surfaces_hidden)
    {
        if(surface->surface()->client()->processId() == pid)
            return surface;
    }
    return nullptr;
}

int WaylandEntries::nbApps() const
{
    QMutexLocker locker(&m_lock);
    return m_surfaces.count();
}
