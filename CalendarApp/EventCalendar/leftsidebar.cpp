#include "leftsidebar.h"
#include "myavailability.h"

#include <QCalendarWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QMenu>
#include <QToolButton>
#include <QDate>
#include <QList>
#include <QAction>
#include <algorithm>

LeftSideBar::LeftSideBar(QWidget *parent)
    : QWidget(parent),
    m_miniCalendar(new QCalendarWidget(this)),
    m_availability(new MyAvailability(this))
{
    // --- 1) Általános beállítások a QCalendarWidget-hez ---
    m_miniCalendar->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
    m_miniCalendar->setGridVisible(false);
    m_miniCalendar->setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
    m_miniCalendar->setFirstDayOfWeek(Qt::Sunday);

    // --- 2) StyleSheet: fehér, minimalista dizájn Unicode nyilakkal ---
    setStyleSheet(R"(
        /* Alap hátteret adunk a teljes LeftSideBar területnek */
        QWidget {
            background-color: #F7F7F7;
        }

        /* Maga a QCalendarWidget doboza */
        QCalendarWidget {
            background-color: #FFFFFF;
            border: 1px solid #DDDDDD;
            border-radius: 8px;
            font-family: "Segoe UI", Tahoma, Geneva, Verdana, sans-serif;
            font-size: 14px;
        }

        /* A navigációs sáv */
        QCalendarWidget QWidget#qt_calendar_navigationbar {
            background-color: #FFFFFF;
            border: none;
            margin: 0;
            padding: 0;
        }

        /* Navigációs gombok */
        QCalendarWidget QToolButton {
            background: transparent;
            border: none;
            color: #333333;
            font-size: 16px;
            font-weight: 600;
            padding: 0 6px;
        }

        QCalendarWidget QToolButton::menu-indicator {
            image: none;
            width: 0px;
        }

        QCalendarWidget QToolButton:hover {
            background-color: #EEEEEE;
            color: #333333;
        }

        /* Előző hónap gomb (balra nyíl) */
        QCalendarWidget QToolButton#qt_calendar_prevmonth {
            qproperty-text: "❮";
            qproperty-icon: none;
            width: 26px;
            height: 26px;
            font-size: 18px;
        }

        /* Következő hónap gomb (jobbra nyíl) */
        QCalendarWidget QToolButton#qt_calendar_nextmonth {
            qproperty-text: "❯";
            qproperty-icon: none;
            width: 26px;
            height: 26px;
            font-size: 18px;
        }

        /* Naptár-nézet */
        QCalendarWidget QAbstractItemView {
            background-color: #FFFFFF;
            color: #333333;
            selection-background-color: #0078D7;
            selection-color: #FFFFFF;
            border: none;
            outline: none;
        }

        /* Hét napjainak fejléce */
        QCalendarWidget QHeaderView::section {
            background-color: #FFFFFF;
            color: #666666;
            font-weight: 600;
            font-size: 12px;
        }

        /* Legördülő menük */
        QMenu {
            background-color: #FFFFFF;
            border: 1px solid #DDDDDD;
            font-family: "Segoe UI", Tahoma, Geneva, Verdana, sans-serif;
            font-size: 13px;
        }
        QMenu::item {
            padding: 5px 20px;
            color: #333333;
            background-color: transparent;
        }
        QMenu::item:selected {
            background-color: #EEEEEE;
            color: #333333;
        }
    )");

    // --- 3) Elrendezés a LeftSideBar-ban ---
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    QLabel* title = new QLabel("Mini Calendar", this);
    title->setStyleSheet("font-weight: bold; font-size: 18px; color: #333333;");
    layout->addWidget(title);

    layout->addWidget(m_miniCalendar);
    layout->addWidget(m_availability, 0, Qt::AlignTop);
    layout->addStretch(1);

    setLayout(layout);

    // --- 4) Jelzések összekötése ---
    connect(m_miniCalendar, &QCalendarWidget::clicked, this, &LeftSideBar::onCalendarClicked);
    connect(m_availability, &MyAvailability::availabilityChanged,
            this, &LeftSideBar::onAvailabilityChanged);

    // --- 5) Év legördülő menü létrehozása a navigációs sávban ---
    QWidget* navBar = m_miniCalendar->findChild<QWidget*>("qt_calendar_navigationbar");
    if (navBar) {
        // Lekérjük az összes QToolButton-t a navigációs sávból
        QList<QToolButton*> buttons = navBar->findChildren<QToolButton*>();
        for (QToolButton* btn : buttons) {
            // Az évjelző gomb az, amelynek a szövege numerikus (pl. "2023")
            bool isNumber;
            int yearValue = btn->text().toInt(&isNumber);
            if (isNumber) {
                // Létrehozunk egy legördülő menüt
                QMenu* yearMenu = new QMenu(btn);
                int currentYear = yearValue;
                // 7 év intervallum: aktuális év -3-tól aktuális év +3-ig
                int startYear = currentYear - 3;
                int endYear = currentYear + 3;
                for (int y = startYear; y <= endYear; ++y) {
                    QAction* act = yearMenu->addAction(QString::number(y));
                    // Kapcsolódás: kiválasztáskor beállítja a naptárat az adott évre
                    connect(act, &QAction::triggered, this, [this, y]() {
                        QDate currentDate = m_miniCalendar->selectedDate();
                        int month = m_miniCalendar->monthShown();
                        // Biztonsági ellenőrzés: ha az aktuális nap nem létezik az új hónapban
                        int day = qMin(currentDate.day(), QDate(y, month, 1).daysInMonth());
                        QDate newDate(y, month, day);
                        m_miniCalendar->setSelectedDate(newDate);
                        m_miniCalendar->setCurrentPage(y, month);
                    });
                }
                // A gombhoz rendeljük a legördülő menüt, és állítsuk be az azonnali megjelenítést
                btn->setMenu(yearMenu);
                btn->setPopupMode(QToolButton::InstantPopup);
            }
        }
    }
}

void LeftSideBar::onCalendarClicked(const QDate &date)
{
    emit dateSelected(date);
}

void LeftSideBar::onAvailabilityChanged(const QStringList &types)
{
    emit availabilityFilterChanged(types);
}
