#ifndef LEFTSIDEBAR_H
#define LEFTSIDEBAR_H

#include <QWidget>
#include <QDate>

class QCalendarWidget;
class MyAvailability;

class LeftSideBar : public QWidget
{
    Q_OBJECT
public:
    explicit LeftSideBar(QWidget *parent = nullptr);

signals:
    void dateSelected(const QDate &date);
    void availabilityFilterChanged(const QStringList &activeTypes);

private slots:
    void onCalendarClicked(const QDate &date);
    void onAvailabilityChanged(const QStringList &types);

private:
    QCalendarWidget* m_miniCalendar;
    MyAvailability* m_availability;
};

#endif // LEFTSIDEBAR_H
