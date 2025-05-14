#include "mainwindow.h"
#include "topbar.h"
#include "leftsidebar.h"
#include "weekview.h"
#include "eventmodel.h"
#include "addeventdialog.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_topBar(new TopBar(this)),
    m_leftSideBar(new LeftSideBar(this)),
    m_weekView(new WeekView(this)),
    m_eventModel(new EventModel(this))
{
    setupUi();
    setupConnections();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    // Fő horizontális layout
    QHBoxLayout* mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Bal oldali sáv (fix szélesség)
    m_leftSideBar->setFixedWidth(250);
    mainLayout->addWidget(m_leftSideBar);

    // Jobb oldalon felül a top bar, alatta a WeekView
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    rightLayout->addWidget(m_topBar);
    rightLayout->addWidget(m_weekView, 1);

    mainLayout->addLayout(rightLayout, 1);

    resize(1400, 900);
    setWindowTitle("Calendar.com Qt Példa");
}

void MainWindow::setupConnections()
{
    // A topBar jelei
    connect(m_topBar, &TopBar::dateChanged, m_weekView, &WeekView::goToDate);
    connect(m_topBar, &TopBar::addEventClicked, [this]() {
        // Felugrik az AddEventDialog
        AddEventDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            auto ev = dialog.event();
            m_eventModel->addEvent(ev);
            m_weekView->refresh(); // Megjelenítjük az új eseményt
        }
    });

    // Bal oldali sáv: miniCalendar -> dateSelected
    connect(m_leftSideBar, &LeftSideBar::dateSelected, m_weekView, &WeekView::goToDate);

    // MyAvailability checkBoxok
    connect(m_leftSideBar, &LeftSideBar::availabilityFilterChanged,
            m_weekView, &WeekView::setAvailabilityFilter);

    // Esemény törlésének kezelése
    connect(m_weekView, &WeekView::requestDeleteEvent, [this](const QString &eventTitle) {
        if (m_eventModel->deleteEvent(eventTitle)) {
            m_weekView->refresh();
        }
    });

    // Esemény módosításának kezelése
    connect(m_weekView, &WeekView::requestModifyEvent, [this](const QString &eventTitle) {
        // Keressük meg az eseményt a modelből (egyszerű példa: az esemény címét használjuk az azonosításhoz)
        Event targetEvent;
        bool found = false;
        for (const Event &ev : m_eventModel->allEvents()) {
            if (ev.title() == eventTitle) {
                targetEvent = ev;
                found = true;
                break;
            }
        }
        if (!found)
            return;

        AddEventDialog dialog(this);
        dialog.setWindowTitle("Modify Event");
        dialog.setEvent(targetEvent); // Pre-fill a dialog a jelenlegi adatokkal
        if (dialog.exec() == QDialog::Accepted) {
            Event modifiedEvent = dialog.event();
            if (m_eventModel->updateEvent(eventTitle, modifiedEvent)) {
                m_weekView->refresh();
            }
        }
    });

    // A WeekView megkapja a modell referenciáját
    m_weekView->setEventModel(m_eventModel);
}
