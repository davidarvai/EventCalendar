#ifndef EVENTMODEL_H
#define EVENTMODEL_H

#include <QObject>
#include <QVector>
#include <QDate>
#include "event.h"

class DatabaseManager;

class EventModel : public QObject
{
    Q_OBJECT
public:
    explicit EventModel(QObject *parent = nullptr);

    void addEvent(const Event &ev);
    QVector<Event> allEvents() const;
    QVector<Event> eventsInRange(const QDate &startDate, const QDate &endDate) const;

    // Új metódusok:
    bool deleteEvent(const QString &title);
    bool updateEvent(const QString &oldTitle, const Event &ev);

private:
    QVector<Event> m_events;
    DatabaseManager* m_dbManager;
};

#endif // EVENTMODEL_H
