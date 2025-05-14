// ... korábbi include-ok
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QVector>
#include "event.h"

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool openDatabase(const QString &dbPath);
    bool createTables();
    bool addEvent(const Event &ev);
    QVector<Event> loadEvents();

    // Új metódusok:
    bool deleteEvent(const QString &title);
    bool updateEvent(const Event &ev, const QString &oldTitle);

private:
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
