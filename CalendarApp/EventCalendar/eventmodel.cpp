#include "eventmodel.h"
#include "databasemanager.h"
#include <QDebug>

EventModel::EventModel(QObject *parent)
    : QObject(parent),
    m_dbManager(new DatabaseManager(this))
{
    // Nyissuk meg az adatbázist (például events.db a futtatható mellett)
    if (m_dbManager->openDatabase("events.db")) {
        // Betöltjük az összes eseményt
        m_events = m_dbManager->loadEvents();
    } else {
        qDebug() << "Failed to open database!";
    }
}

void EventModel::addEvent(const Event &ev)
{
    m_events.append(ev);
    // Az adatbázisba is mentsük
    if (!m_dbManager->addEvent(ev)) {
        qDebug() << "Failed to save event to database";
    }
}

QVector<Event> EventModel::allEvents() const
{
    return m_events;
}

QVector<Event> EventModel::eventsInRange(const QDate &startDate, const QDate &endDate) const
{
    QVector<Event> result;
    for (const Event &ev : m_events) {
        QDate d = ev.start().date();
        if (d >= startDate && d <= endDate) {
            result.append(ev);
        }
    }
    return result;
}

bool EventModel::deleteEvent(const QString &title)
{
    bool ok = m_dbManager->deleteEvent(title);
    if (ok) {
        for (int i = 0; i < m_events.size(); ++i) {
            if (m_events.at(i).title() == title) {
                m_events.remove(i);
                break;
            }
        }
    }
    return ok;
}

bool EventModel::updateEvent(const QString &oldTitle, const Event &ev)
{
    bool ok = m_dbManager->updateEvent(ev, oldTitle);
    if (ok) {
        for (int i = 0; i < m_events.size(); ++i) {
            if (m_events.at(i).title() == oldTitle) {
                m_events[i] = ev;
                break;
            }
        }
    }
    return ok;
}

