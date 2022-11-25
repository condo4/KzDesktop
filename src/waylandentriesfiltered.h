#ifndef WAYLANDENTIRESFILTERED_H
#define WAYLANDENTIRESFILTERED_H

#include <QSortFilterProxyModel>
#include <QObject>

class WaylandEntries;
namespace KaZoe {
    class DesktopEntries;
}

class WaylandEntriesFiltered : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QObject *model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QString roleFilter READ roleFilter WRITE setRoleFilter NOTIFY roleFilterChanged)
    Q_PROPERTY(int length READ length NOTIFY lengthChanged)
    KaZoe::DesktopEntries *m_entries {nullptr};

public:
    explicit WaylandEntriesFiltered(QObject *parent = nullptr);
    QString roleFilter() const;
    void setRoleFilter(const QString &roles);
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

    QObject *model() const;
    void setModel(QObject *newModel);

    int length();

signals:
    void roleFilterChanged();
    void modelChanged(QObject * model);
    void lengthChanged();

private:
    QStringList m_roleFilter;
    WaylandEntries *m_model = nullptr;
};

#endif // WAYLANDENTIRESFILTERED_H
