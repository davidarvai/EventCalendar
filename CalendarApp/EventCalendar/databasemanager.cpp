#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "eventtype.h"

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen())
        m_db.close();
}

bool DatabaseManager::openDatabase(const QString &dbPath)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);
    if (!m_db.open()) {
        qDebug() << "Database open error:" << m_db.lastError().text();
        return false;
    }
    return createTables();
}

bool DatabaseManager::createTables()
{
    QSqlQuery query;
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS events (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            start TEXT NOT NULL,
            end TEXT NOT NULL,
            type TEXT NOT NULL,
            location TEXT
        )
    )";
    if (!query.exec(sql)) {
        qDebug() << "Create table error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::addEvent(const Event &ev)
{
    QSqlQuery query;
    query.prepare("INSERT INTO events (title, start, end, type, location) "
                  "VALUES (:title, :start, :end, :type, :location)");
    query.bindValue(":title", ev.title());
    query.bindValue(":start", ev.start().toString(Qt::ISODate));
    query.bindValue(":end",   ev.end().toString(Qt::ISODate));
    query.bindValue(":type",  eventTypeToString(ev.type()));
    query.bindValue(":location", ev.location());

    if (!query.exec()) {
        qDebug() << "Add event error:" << query.lastError().text();
        return false;
    }
    return true;
}

QVector<Event> DatabaseManager::loadEvents()
{
    QVector<Event> events;
    QSqlQuery query("SELECT title, start, end, type, location FROM events");
    while (query.next()) {
        QString title = query.value(0).toString();
        QDateTime start = QDateTime::fromString(query.value(1).toString(), Qt::ISODate);
        QDateTime end   = QDateTime::fromString(query.value(2).toString(), Qt::ISODate);
        QString typeStr = query.value(3).toString();
        QString location = query.value(4).toString();

        EventType et = eventTypeFromString(typeStr);
        events.append(Event(title, start, end, et, location));
    }
    return events;
}

bool DatabaseManager::deleteEvent(const QString &title)
{
    QSqlQuery query;
    query.prepare("DELETE FROM events WHERE title = :title");
    query.bindValue(":title", title);
    if (!query.exec()) {
        qDebug() << "Delete event error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::updateEvent(const Event &ev, const QString &oldTitle)
{
    QSqlQuery query;
    query.prepare("UPDATE events SET title = :title, start = :start, end = :end, type = :type, location = :location WHERE title = :oldTitle");
    query.bindValue(":title", ev.title());
    query.bindValue(":start", ev.start().toString(Qt::ISODate));
    query.bindValue(":end", ev.end().toString(Qt::ISODate));
    query.bindValue(":type", eventTypeToString(ev.type()));
    query.bindValue(":location", ev.location());
    query.bindValue(":oldTitle", oldTitle);
    if (!query.exec()) {
        qDebug() << "Update event error:" << query.lastError().text();
        return false;
    }
    return true;
}
