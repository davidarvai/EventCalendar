#include "scheduleview.h"
#include "eventmodel.h"  // Győződj meg róla, hogy ez a fájl is megfelelően van definiálva
#include "event.h"       // Ha szükséges, az eseményekhez
#include "viewmode.h"    // Bár ez már include-olva van a headerben, itt is lehet, ha szükséges
#include <QPainter>
#include <QDateTime>
#include <QTime>

ScheduleView::ScheduleView(QWidget *parent)
    : QWidget(parent),
    m_eventModel(nullptr),
    m_currentViewMode(ViewMode::Week),  // Alapértelmezett mód: Heti nézet
    m_currentDate(QDate::currentDate())
{
    // Egyéb inicializációs kód, ha szükséges
}

void ScheduleView::setEventModel(EventModel* model)
{
    m_eventModel = model;
    updateView();
}

void ScheduleView::setViewMode(ViewMode mode)
{
    m_currentViewMode = mode;
    updateView();
}

void ScheduleView::goToDate(const QDate &date)
{
    m_currentDate = date;
    updateView();
}

void ScheduleView::updateView()
{
    // Egyszerűen újrarajzoltatjuk a widgetet
    update();
}

void ScheduleView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Méretek
    int w = width();
    int h = height();

    // Margók
    int leftMargin = 50;  // Hely az órák kijelzésére
    int topMargin = 30;   // Hely a napneveknek
    int rightMargin = 20;
    int bottomMargin = 20;

    int usableWidth = w - leftMargin - rightMargin;
    int usableHeight = h - topMargin - bottomMargin;

    // Egy nap szélessége és egy óra magassága
    float dayWidth = static_cast<float>(usableWidth) / 7.0f;
    float hourHeight = static_cast<float>(usableHeight) / 24.0f;

    // Háttér
    p.fillRect(rect(), Qt::white);

    // Napok fejléce: például "Mon\nMar 27"
    for (int day = 0; day < 7; day++) {
        // Itt feltételezzük, hogy m_currentDate a hét első napja (pl. hétfő)
        QDate d = m_currentDate.addDays(day);
        QString dayName = d.toString("ddd\nMMM d");
        float x = leftMargin + dayWidth * day;
        QRectF r(x, 0, dayWidth, topMargin);
        p.drawText(r, Qt::AlignCenter, dayName);
    }

    // Órák kijelzése
    for (int hour = 0; hour <= 24; hour++) {
        float y = topMargin + hourHeight * hour;
        p.setPen(QColor("#EEEEEE"));
        p.drawLine(leftMargin, y, w - rightMargin, y);
        if (hour < 24) {
            QString hourStr = QString("%1:00").arg(hour, 2, 10, QLatin1Char('0'));
            QRectF r(0, y - hourHeight, leftMargin - 5, hourHeight);
            p.setPen(Qt::black);
            p.drawText(r, Qt::AlignRight | Qt::AlignVCenter, hourStr);
        }
    }

    // Függőleges elválasztó vonalak a napok között
    p.setPen(QColor("#DDDDDD"));
    for (int day = 0; day <= 7; day++) {
        float x = leftMargin + dayWidth * day;
        p.drawLine(x, topMargin, x, topMargin + 24 * hourHeight);
    }

    // (Itt megjelenítheted az eseményeket, ha m_eventModel nem nullptr)
    // Például:
    // if (m_eventModel) {
    //     auto events = m_eventModel->eventsInRange(...);
    //     // Rajzolás kódja...
    // }

    // Aktuális idő vonal (piros)
    QDateTime now = QDateTime::currentDateTime();
    // Ha az aktuális nap az adott hétben van
    if (now.date() == m_currentDate) {
        float currentHour = now.time().hour() + now.time().minute() / 60.0f;
        float y = topMargin + hourHeight * currentHour;
        p.setPen(QPen(Qt::red, 2));
        p.drawLine(leftMargin, y, w - rightMargin, y);
    }
}
