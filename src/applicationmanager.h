#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include <QObject>

class ApplicationManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *currentSurface READ currentSurface WRITE setCurrentSurface NOTIFY currentSurfaceChanged)
    Q_PROPERTY(bool isHome READ isHome WRITE setIsHome NOTIFY isHomeChanged)
    Q_PROPERTY(QString virtualkeyboard READ virtualkeyboard CONSTANT)

    QObject *m_currentSurface {nullptr};
    bool m_isHome;

public:
    explicit ApplicationManager(QObject *parent = nullptr);

    bool isHome() const;

public slots:
    QObject *currentSurface() const;
    void setCurrentSurface(QObject *newCurrentSurface);

    int count();
    QString applicationName(int id);
    int applicationPid(int id);
    int applicationFocus(int id);
    int currentApplicationPid();
    void hideApplicationPid(qint64 pid);
    void raiseApplicationPid(qint64 pid);
    void backHome();
    void setIsHome(bool isHome);

    QString virtualkeyboard();

signals:
    void currentSurfaceChanged();
    void gotoHome();
    void isHomeChanged(bool isHome);
};

#endif // APPLICATIONMANAGER_H
