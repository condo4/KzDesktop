#ifndef WAYLANDENTRIES_H
#define WAYLANDENTRIES_H

#include <QAbstractListModel>
#include <QRecursiveMutex>
#include <QObject>

class QWaylandXdgSurface;

class WaylandEntries : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int nbApps READ nbApps NOTIFY nbAppsChanged)


    QList<QWaylandXdgSurface *> m_surfaces;
    QList<QWaylandXdgSurface *> m_surfaces_hidden;
    QList<qint64> m_hidden;
    mutable QRecursiveMutex m_lock;

public:
    explicit WaylandEntries(QObject *parent = nullptr);

    enum WaylandEntryRoles {
        SurfaceRole = Qt::UserRole + 1,
        TitleRole,
        PidRole,
        AppIdRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    static WaylandEntries* getInstance();

    int nbApps() const;
    void setNbApps(int newNbApps);

public slots:
    void addSurface(QObject *obj);
    void addHidePid(qint64 pid);
    void surfaceDestroyed();
    void stop(int pid);
    QObject *getSurface(int pid);

signals:
    void nbAppsChanged();
    void topWindowRegistered(QObject *surface, QString appid);
    void topWindowDestroyed(QObject *surface, QString appid);
};

#endif // WAYLANDENTRIES_H
