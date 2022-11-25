#include "waylandentriesfiltered.h"
#include "waylandentries.h"
#include "xdgdesktopentries.h"
#include <QDebug>

WaylandEntriesFiltered::WaylandEntriesFiltered(QObject *parent)
    : QSortFilterProxyModel{parent}
{
    m_entries = new KaZoe::DesktopEntries(this);
    QObject::connect(this, &WaylandEntriesFiltered::rowsInserted, this, &WaylandEntriesFiltered::lengthChanged);
    QObject::connect(this, &WaylandEntriesFiltered::rowsRemoved, this, &WaylandEntriesFiltered::lengthChanged);
}

QString WaylandEntriesFiltered::roleFilter() const
{
    return m_roleFilter.join(";");
}

void WaylandEntriesFiltered::setRoleFilter(const QString &roles)
{
    QStringList newlist = roles.split(";");
    if (m_roleFilter == newlist)
        return;
    m_roleFilter = newlist;
    emit roleFilterChanged();
}

bool WaylandEntriesFiltered::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    QVariant appid = sourceModel()->data(index, WaylandEntries::AppIdRole);
    QString role = m_entries->appIdData(appid.toString(), "Desktop Entry/X-DESKTOPMGR-ROLE");

    if(appid.isNull())
        return false;

    if(m_roleFilter.contains(role))
    {
        return false;
    }
    return true;
}

QObject *WaylandEntriesFiltered::model() const
{
    return static_cast<QObject*>(m_model);
}

void WaylandEntriesFiltered::setModel(QObject *newModel)
{
    WaylandEntries *entries = qobject_cast<WaylandEntries*>(newModel);
    if (entries)
    {
        m_model = entries;
        setSourceModel(m_model);
        emit modelChanged(m_model);
    }
}

int WaylandEntriesFiltered::length()
{
    return rowCount();
}
