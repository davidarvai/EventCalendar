#include "weekview.h"
#include "eventmodel.h"
#include "event.h"
#include "eventtype.h"

#include <QPainter>
#include <QDateTime>
#include <QTime>
#include <QLinearGradient>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QToolTip>
#include <QDebug>
#include <QScrollBar>
#include <QResizeEvent>

// Modern színséma és elrendezési konstansok
namespace {
const QColor BACKGROUND_TOP = QColor("#F8FBFF");
const QColor BACKGROUND_BOTTOM = QColor("#EFF4F9");
const QColor HOUR_LINE_COLOR = QColor(220, 228, 236);
const QColor DAY_SEPARATOR_COLOR = QColor(200, 210, 220);
const QColor HEADER_BACKGROUND = QColor("#5A7B9E");
const QColor CURRENT_TIME_COLOR = QColor("#4285F4");
const QColor TEXT_COLOR = QColor("#2D3844");

const int leftMargin = 70;
const int topMargin = 60;
const int rightMargin = 20;
const int bottomMargin = 30;
const int headerHeight = 50;
const int hourHeight = 80;
const int MIN_DAY_WIDTH = 150;
}

WeekView::WeekView(QWidget *parent)
    : QAbstractScrollArea(parent),
    m_eventModel(nullptr)
{
    // Állítsuk be a jelenlegi hét első napját (hétfő)
    QDate today = QDate::currentDate();
    QDate monday = today;
    while (monday.dayOfWeek() != 1) {
        monday = monday.addDays(-1);
    }
    m_referenceDate = monday;

    // Kezdetben az összes típus aktív
    m_activeTypes << "Meeting" << "Workout" << "Personal";

    // Engedélyezzük az egérmozgás követését a viewport-on
    viewport()->setMouseTracking(true);

    // Görgetősávok mindig láthatóak
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    // Induláskor frissítsük a scroll sávokat és a dayWidth-et
    updateScrollbarsAndDayWidth();
}

void WeekView::resizeEvent(QResizeEvent *event)
{
    QAbstractScrollArea::resizeEvent(event);
    // Ablakméret változásakor frissítjük a dayWidth-et és a scrollokat
    updateScrollbarsAndDayWidth();
}

void WeekView::updateScrollbarsAndDayWidth()
{
    // Minimális szélesség a 7 napra (7 * MIN_DAY_WIDTH), plusz a margók
    int minTotalWidth = leftMargin + MIN_DAY_WIDTH * 7 + rightMargin;
    int viewWidth = viewport()->width();

    if (viewWidth < minTotalWidth) {
        m_dayWidth = double(MIN_DAY_WIDTH);
        horizontalScrollBar()->setRange(0, minTotalWidth - viewWidth);
    } else {
        horizontalScrollBar()->setRange(0, 0);
        int availableWidth = viewWidth - leftMargin - rightMargin;
        m_dayWidth = double(availableWidth) / 7.0;
    }

    // Függőleges görgetés kiszámítása
    int totalHeight = topMargin + hourHeight * 24 + bottomMargin;
    if (totalHeight > viewport()->height()) {
        verticalScrollBar()->setRange(0, totalHeight - viewport()->height());
    } else {
        verticalScrollBar()->setRange(0, 0);
    }
    viewport()->update();
}

void WeekView::setEventModel(EventModel *model)
{
    m_eventModel = model;
    updateView();
}

void WeekView::goToDate(const QDate &date)
{
    // Az adott dátumhoz tartozó hétfő meghatározása
    QDate monday = date;
    while (monday.dayOfWeek() != 1) {
        monday = monday.addDays(-1);
    }
    m_referenceDate = monday;
    updateView();
}

void WeekView::setAvailabilityFilter(const QStringList &types)
{
    m_activeTypes = types;
    updateView();
}

void WeekView::updateView()
{
    viewport()->update();
}

QSize WeekView::sizeHint() const
{
    int totalWidth = leftMargin + MIN_DAY_WIDTH * 7 + rightMargin;
    int totalHeight = topMargin + hourHeight * 24 + bottomMargin;
    return QSize(totalWidth, totalHeight);
}

void WeekView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(viewport());
    p.setRenderHint(QPainter::Antialiasing, true);

    // Háttér rajzolása
    QLinearGradient bgGradient(0, 0, 0, viewport()->height());
    bgGradient.setColorAt(0, BACKGROUND_TOP);
    bgGradient.setColorAt(1, BACKGROUND_BOTTOM);
    p.save();
    p.resetTransform();
    p.fillRect(viewport()->rect(), bgGradient);
    p.restore();

    int hScroll = horizontalScrollBar()->value();
    int vScroll = verticalScrollBar()->value();

    // --- Header (napfejléc) rajzolása: csak vízszintes görgetéssel ---
    p.save();
    p.translate(-hScroll, 0);
    QFont headerFont("Segoe UI", 12, QFont::Medium);
    p.setFont(headerFont);
    double dayW = m_dayWidth;

    for (int day = 0; day < 7; day++) {
        QDate d = m_referenceDate.addDays(day);
        QRect headerRect(
            leftMargin + int(dayW * day),
            topMargin - headerHeight,
            int(dayW),
            headerHeight
            );

        QLinearGradient grad(headerRect.topLeft(), headerRect.bottomLeft());
        grad.setColorAt(0, HEADER_BACKGROUND.lighter(110));
        grad.setColorAt(1, HEADER_BACKGROUND);
        p.fillRect(headerRect, grad);

        QString dayText = d.toString("ddd").toUpper();
        QString dateText = d.toString("d MMM");

        p.setPen(Qt::white);
        p.drawText(headerRect.adjusted(0, 5, 0, -15), Qt::AlignCenter, dayText);

        QFont dateFont("Segoe UI", 10);
        p.setFont(dateFont);
        p.drawText(headerRect.adjusted(0, 25, 0, 0), Qt::AlignCenter, dateText);
        p.setFont(headerFont);
    }
    p.restore();

    // --- Tartalom (grid, órák, események, stb.) rajzolása: mindkét irányban eltolva ---
    p.save();
    p.translate(-hScroll, -vScroll);

    // Óravonalak és óra címkék
    QFont hourFont("Segoe UI", 10);
    p.setFont(hourFont);
    p.setPen(HOUR_LINE_COLOR);

    for (int hour = 0; hour <= 24; hour++) {
        int y = topMargin + hourHeight * hour;
        p.drawLine(leftMargin, y, leftMargin + int(dayW) * 7, y);

        if (hour < 24) {
            QString hourStr = QString("%1:00").arg(hour, 2, 10, QLatin1Char('0'));
            QRect hourRect(0, y, leftMargin - 15, hourHeight);
            p.setPen(QColor(150, 160, 170));
            p.drawText(hourRect, Qt::AlignRight | Qt::AlignVCenter, hourStr);
            p.setPen(HOUR_LINE_COLOR);
        }
    }

    // Nap elválasztó vonalak
    p.setPen(DAY_SEPARATOR_COLOR);
    for (int day = 0; day <= 7; day++) {
        int x = leftMargin + int(dayW * day);
        p.drawLine(x, topMargin, x, topMargin + hourHeight * 24);
    }

    // Jelenlegi idő vonal
    QDateTime now = QDateTime::currentDateTime();
    float currentHour = now.time().hour() + now.time().minute() / 60.0f;
    int lineY = topMargin + int(hourHeight * currentHour);
    p.setPen(QPen(CURRENT_TIME_COLOR, 2, Qt::DashLine));
    p.drawLine(leftMargin, lineY, leftMargin + int(dayW) * 7, lineY);

    // Idő címke rajzolása
    QString timeLabel = now.toString("HH:mm");
    QRect labelRect(leftMargin - 60, lineY - 11, 50, 22);
    p.setBrush(CURRENT_TIME_COLOR);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(labelRect, 4, 4);
    p.setPen(Qt::white);
    p.drawText(labelRect, Qt::AlignCenter, timeLabel);

    // Események rajzolása
    if (m_eventModel) {
        QDate startOfWeek = m_referenceDate;
        QDate endOfWeek = m_referenceDate.addDays(6);
        auto events = m_eventModel->eventsInRange(startOfWeek, endOfWeek);

        for (const Event &ev : events) {
            QString typeStr = eventTypeToString(ev.type());
            if (!m_activeTypes.contains(typeStr))
                continue;

            int dayOffset = m_referenceDate.daysTo(ev.start().date());
            if (dayOffset < 0 || dayOffset > 6)
                continue;

            float startHour = ev.start().time().hour() + ev.start().time().minute() / 60.0f;
            float endHour = ev.end().time().hour() + ev.end().time().minute() / 60.0f;
            float duration = endHour - startHour;
            if (duration <= 0)
                duration = 0.5f;

            int eventX = leftMargin + int(dayW * dayOffset) + 4;
            int eventY = topMargin + int(hourHeight * startHour) + 4;
            int eventW = int(dayW) - 8;
            int eventH = int(hourHeight * duration) - 8;

            QColor baseColor = eventTypeColor(ev.type());
            QLinearGradient eventGrad(eventX, eventY, eventX, eventY + eventH);
            eventGrad.setColorAt(0, baseColor.lighter(110));
            eventGrad.setColorAt(1, baseColor);

            p.setPen(Qt::NoPen);
            p.setBrush(eventGrad);
            p.drawRoundedRect(eventX, eventY, eventW, eventH, 6, 6);

            p.setPen(QColor(0, 0, 0, 30));
            p.drawLine(eventX + 2, eventY + eventH + 1, eventX + eventW - 2, eventY + eventH + 1);

            p.setPen(TEXT_COLOR);
            QFont eventFont("Segoe UI", 10);
            p.setFont(eventFont);

            QRect textRect(eventX + 8, eventY + 6, eventW - 16, eventH - 12);
            QString title = p.fontMetrics().elidedText(ev.title(), Qt::ElideRight, textRect.width());
            p.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, title);

            QFont timeFont("Segoe UI", 8);
            p.setFont(timeFont);
            QString timeRange = ev.start().time().toString("HH:mm") + " - " + ev.end().time().toString("HH:mm");
            p.drawText(textRect.adjusted(0, 15, 0, 0), timeRange);
        }
    }
    p.restore();
}

bool WeekView::findEventAt(const QPoint &pos, Event &foundEvent) const
{
    if (!m_eventModel)
        return false;

    // Figyelembe vesszük a görgetést
    QPoint adjustedPos = pos + QPoint(horizontalScrollBar()->value(), verticalScrollBar()->value());
    QDate startOfWeek = m_referenceDate;
    QDate endOfWeek = m_referenceDate.addDays(6);
    auto events = m_eventModel->eventsInRange(startOfWeek, endOfWeek);

    for (const Event &ev : events) {
        QString typeStr = eventTypeToString(ev.type());
        if (!m_activeTypes.contains(typeStr))
            continue;

        int dayOffset = m_referenceDate.daysTo(ev.start().date());
        if (dayOffset < 0 || dayOffset > 6)
            continue;

        float startHour = ev.start().time().hour() + ev.start().time().minute() / 60.0f;
        float endHour = ev.end().time().hour() + ev.end().time().minute() / 60.0f;
        float duration = endHour - startHour;
        if (duration <= 0)
            duration = 0.5f;

        QRect eventRect(
            leftMargin + int(m_dayWidth * dayOffset) + 4,
            topMargin + int(hourHeight * startHour) + 4,
            int(m_dayWidth) - 8,
            int(hourHeight * duration) - 8
            );

        if (eventRect.contains(adjustedPos)) {
            foundEvent = ev;
            return true;
        }
    }
    return false;
}

void WeekView::contextMenuEvent(QContextMenuEvent *event)
{
    Event ev;
    if (findEventAt(event->pos(), ev)) {
        QMenu menu(viewport());
        QAction *modifyAction = menu.addAction("Modify Event");
        QAction *deleteAction = menu.addAction("Delete Event");
        QAction *selectedAction = menu.exec(event->globalPos());
        if (selectedAction == modifyAction) {
            emit requestModifyEvent(ev.title());
        } else if (selectedAction == deleteAction) {
            emit requestDeleteEvent(ev.title());
        }
    }
}

void WeekView::mouseMoveEvent(QMouseEvent *event)
{
    Event ev;
    if (findEventAt(event->pos(), ev)) {
        QString detailText = QString("Title: %1\nStart: %2\nEnd: %3\nType: %4\nLocation: %5")
        .arg(ev.title())
            .arg(ev.start().toString("yyyy-MM-dd HH:mm"))
            .arg(ev.end().toString("yyyy-MM-dd HH:mm"))
            .arg(eventTypeToString(ev.type()))
            .arg(ev.location());
        QToolTip::showText(event->globalPos(), detailText, viewport());
    } else {
        QToolTip::hideText();
        event->ignore();
    }
    QAbstractScrollArea::mouseMoveEvent(event);
}

void WeekView::showEvent(QShowEvent *event)
{
    QAbstractScrollArea::showEvent(event);
    QDateTime now = QDateTime::currentDateTime();
    float currentHour = now.time().hour() + now.time().minute() / 60.0f;
    int lineY = topMargin + int(hourHeight * currentHour);
    verticalScrollBar()->setValue(qMax(0, lineY - viewport()->height() / 2));
}
