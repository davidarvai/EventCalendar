#ifndef SCHEDULEVIEW_H
#define SCHEDULEVIEW_H

#include <QWidget>
#include <QDate>
#include "viewmode.h"   // Importáljuk a ViewMode definíciót

class EventModel;

class ScheduleView : public QWidget
{
    Q_OBJECT
public:
    explicit ScheduleView(QWidget *parent = nullptr);

    void setEventModel(EventModel* model);

public slots:
    void setViewMode(ViewMode mode);
    void goToDate(const QDate &date);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    EventModel* m_eventModel;
    ViewMode m_currentViewMode; // Az enum itt már ismert, mert include-oltuk a viewmode.h-t
    QDate m_currentDate;

    void updateView(); // Belső frissítés
};

#endif // SCHEDULEVIEW_H
