#ifndef WEEKVIEW_H
#define WEEKVIEW_H

#include <QAbstractScrollArea>
#include <QDate>
#include <QStringList>
#include "eventtype.h"

#ifdef _WIN32
#undef Event
#endif

#include "event.h"  // Saját Event osztály

class EventModel;

class WeekView : public QAbstractScrollArea
{
    Q_OBJECT
public:
    explicit WeekView(QWidget *parent = nullptr);

    void setEventModel(EventModel* model);
    void goToDate(const QDate &date);
    void setAvailabilityFilter(const QStringList &types);
    void refresh() { updateView(); }

signals:
    void requestDeleteEvent(const QString &eventTitle);
    void requestModifyEvent(const QString &eventTitle);

protected:
    void paintEvent(QPaintEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;  // Új: alkalmazás megjelenésekor a scroll beállítása
    QSize sizeHint() const override;

private:
    EventModel* m_eventModel;
    QDate m_referenceDate;         // A hét kezdő napja (hétfő)
    QStringList m_activeTypes;     // Aktív eseménytípusok

    // Dinamikus oszlopszélesség (legalább 150 px)
    double m_dayWidth = 150.0;

    void updateView();
    bool findEventAt(const QPoint &pos, Event &foundEvent) const;

    // Segédfüggvény a görgetősávok és a dayWidth frissítéséhez
    void updateScrollbarsAndDayWidth();
};

#endif // WEEKVIEW_H
