#ifndef EVENTTYPE_H
#define EVENTTYPE_H

#include <QString>
#include <QColor>

/*!
 * \brief Három példa eseménytípus: Meeting, Workout, Personal
 */
enum class EventType {
    Meeting,
    Workout,
    Personal,
    Unknown
};

/*!
 * \brief Eseménytípus -> Szöveg
 */
inline QString eventTypeToString(EventType t)
{
    switch (t) {
    case EventType::Meeting:  return "Meeting";
    case EventType::Workout:  return "Workout";
    case EventType::Personal: return "Personal";
    default:                  return "Unknown";
    }
}

/*!
 * \brief Szöveg -> Eseménytípus
 */
inline EventType eventTypeFromString(const QString &str)
{
    if (str == "Meeting")  return EventType::Meeting;
    if (str == "Workout")  return EventType::Workout;
    if (str == "Personal") return EventType::Personal;
    return EventType::Unknown;
}

/*!
 * \brief Az egyes eseménytípusok színei
 */
inline QColor eventTypeColor(EventType t)
{
    switch (t) {
    case EventType::Meeting:  return QColor("#B3E6B3"); // világoszöld
    case EventType::Workout:  return QColor("#FFB3B3"); // világospiros
    case EventType::Personal: return QColor("#FFF2B3"); // világossárga
    default:                  return QColor("#CCCCCC"); // semleges szürke
    }
}

#endif // EVENTTYPE_H
