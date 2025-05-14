#include "topbar.h"
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QDate>

TopBar::TopBar(QWidget *parent)
    : QWidget(parent),
    m_btnPrev(new QPushButton("<", this)),
    m_btnNext(new QPushButton(">", this)),
    m_btnToday(new QPushButton("This Week", this)),
    m_btnAddEvent(new QPushButton("+", this)),
    m_dateLabel(new QLabel(this)),
    m_currentDate(QDate::currentDate())
{
    // Stílus beállítás
    setStyleSheet(R"(
        QWidget {
            background-color: #F7F7F7;       /* Világos háttér */
            border-bottom: 1px solid #DDDDDD;/* Vékony alsó keret */
        }
        QPushButton {
            border: none;
            background-color: transparent;
            font-size: 14px;
            padding: 6px 12px;
            color: #333333;
        }
        QPushButton:hover {
            background-color: #E0E0E0;
        }
        #addEventButton {
            background-color: #0066FF;
            color: white;
            border-radius: 16px;
            width: 32px;
            height: 32px;
            font-size: 20px;
            font-weight: bold;
        }
        QLabel {
            font-size: 16px;
            font-weight: bold;
            color: #333333;
        }
    )");

    // A kék plusz gomb stílusa
    m_btnAddEvent->setObjectName("addEventButton");

    // Elrendezés
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setSpacing(10);

    // Bal oldalon: Prev, Next, Today
    layout->addWidget(m_btnPrev);
    layout->addWidget(m_btnNext);
    layout->addWidget(m_btnToday);

    // Középen a dátumcímke
    layout->addStretch(1);
    layout->addWidget(m_dateLabel);
    layout->addStretch(1);

    // Jobb oldalon a plusz gomb
    layout->addWidget(m_btnAddEvent);

    setLayout(layout);

    // Kapcsolatok
    connect(m_btnPrev, &QPushButton::clicked, this, &TopBar::onPrevWeekClicked);
    connect(m_btnNext, &QPushButton::clicked, this, &TopBar::onNextWeekClicked);
    connect(m_btnToday, &QPushButton::clicked, this, &TopBar::onTodayClicked);
    connect(m_btnAddEvent, &QPushButton::clicked, this, &TopBar::onAddEventClicked);

    updateDateLabel();
}

void TopBar::onTodayClicked()
{
    m_currentDate = QDate::currentDate();
    updateDateLabel();
    emit dateChanged(m_currentDate);
}

void TopBar::onPrevWeekClicked()
{
    m_currentDate = m_currentDate.addDays(-7);
    updateDateLabel();
    emit dateChanged(m_currentDate);
}

void TopBar::onNextWeekClicked()
{
    m_currentDate = m_currentDate.addDays(7);
    updateDateLabel();
    emit dateChanged(m_currentDate);
}

void TopBar::onAddEventClicked()
{
    emit addEventClicked();
}

void TopBar::updateDateLabel()
{
    // Megkeressük a hétfőt
    QDate monday = m_currentDate;
    while (monday.dayOfWeek() != 1) {
        monday = monday.addDays(-1);
    }
    QDate sunday = monday.addDays(6);

    // Ha ugyanaz a hónap, pl. "June 26 - 30"
    // Különböző hónap esetén pl. "June 29 - July 2"
    QString text;
    if (monday.month() == sunday.month()) {
        text = QString("%1 %2 - %3")
        .arg(monday.toString("MMMM"))    // "June"
            .arg(monday.day())               // 26
            .arg(sunday.day());              // 30
    } else {
        text = QString("%1 %2 - %3 %4")
        .arg(monday.toString("MMM"))     // "Jun"
            .arg(monday.day())               // 29
            .arg(sunday.toString("MMM"))     // "Jul"
            .arg(sunday.day());              // 2
    }

    m_dateLabel->setText(text);
}
