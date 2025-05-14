#ifndef EVENT_H
#define EVENT_H

#include <QDateTime>
#include <QString>
#include "eventtype.h"

class Event
{
public:
    Event() {}
    Event(const QString &title,
          const QDateTime &start,
          const QDateTime &end,
          EventType type = EventType::Unknown,
          const QString &location = "",
          bool isRecurring = false,
          int repeatDayOfWeek = 0)
        : m_title(title),
        m_start(start),
        m_end(end),
        m_type(type),
        m_location(location),
        m_isRecurring(isRecurring),
        m_repeatDayOfWeek(repeatDayOfWeek)
    {}

    // Getterek/setterek
    QString title() const { return m_title; }
    QDateTime start() const { return m_start; }
    QDateTime end() const { return m_end; }
    EventType type() const { return m_type; }
    QString location() const { return m_location; }
    bool isRecurring() const { return m_isRecurring; }
    int repeatDayOfWeek() const { return m_repeatDayOfWeek; }

    void setTitle(const QString &t) { m_title = t; }
    void setStart(const QDateTime &dt) { m_start = dt; }
    void setEnd(const QDateTime &dt) { m_end = dt; }
    void setType(EventType t) { m_type = t; }
    void setLocation(const QString &loc) { m_location = loc; }
    void setIsRecurring(bool r) { m_isRecurring = r; }
    void setRepeatDayOfWeek(int d) { m_repeatDayOfWeek = d; }

private:
    QString m_title;
    QDateTime m_start;
    QDateTime m_end;
    EventType m_type;
    QString m_location;

    // ÚJ mezők
    bool m_isRecurring;         // Igaz, ha heti ismétlődés
    int m_repeatDayOfWeek;      // 1=Hétfő, 2=Kedd, ..., 7=Vasárnap
};

#endif // EVENT_H
