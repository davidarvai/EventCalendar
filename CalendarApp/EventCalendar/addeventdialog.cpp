#include "addeventdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDateEdit>
#include <QTimeEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QCheckBox>
#include "eventtype.h"

AddEventDialog::AddEventDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Add New Event");
    setModal(true);

    setStyleSheet(R"(
        QDialog { background-color: #FFFFFF; }
        QLabel { font-size: 14px; color: #333333; }
        QLineEdit, QDateEdit, QTimeEdit, QComboBox {
            font-size: 14px;
            padding: 8px;
            border: 1px solid #DDDDDD;
            border-radius: 6px;
            background-color: #F5F5F5;
        }
        QCheckBox {
            font-size: 14px;
            color: #333333;
        }
        QDialogButtonBox QPushButton {
            padding: 8px 16px;
            font-size: 14px;
            border: 1px solid #007BFF;
            border-radius: 6px;
            background-color: #007BFF;
            color: white;
        }
        QDialogButtonBox QPushButton:hover {
            background-color: #0056b3;
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout;
    formLayout->setLabelAlignment(Qt::AlignRight);
    formLayout->setSpacing(15);

    // -- Title --
    m_titleEdit = new QLineEdit(this);
    formLayout->addRow("Title:", m_titleEdit);

    // -- Start date + time --
    QHBoxLayout *startLayout = new QHBoxLayout;
    m_startDateEdit = new QDateEdit(QDate::currentDate(), this);
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setDisplayFormat("yyyy-MM-dd");
    m_startTimeEdit = new QTimeEdit(QTime::currentTime(), this);
    m_startTimeEdit->setDisplayFormat("HH:mm");
    startLayout->addWidget(m_startDateEdit);
    startLayout->addWidget(m_startTimeEdit);
    formLayout->addRow("Start:", startLayout);

    // -- End date + time --
    QHBoxLayout *endLayout = new QHBoxLayout;
    m_endDateEdit = new QDateEdit(QDate::currentDate(), this);
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDisplayFormat("yyyy-MM-dd");
    m_endTimeEdit = new QTimeEdit(QTime::currentTime().addSecs(3600), this);
    m_endTimeEdit->setDisplayFormat("HH:mm");
    endLayout->addWidget(m_endDateEdit);
    endLayout->addWidget(m_endTimeEdit);
    formLayout->addRow("End:", endLayout);

    // -- Location --
    m_locationEdit = new QLineEdit(this);
    formLayout->addRow("Location:", m_locationEdit);

    // -- Event Type --
    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItem("Meeting");
    m_typeCombo->addItem("Workout");
    m_typeCombo->addItem("Personal");
    formLayout->addRow("Event Type:", m_typeCombo);

    // -- Recurring checkbox --
    m_recurringCheck = new QCheckBox("Weekly recurring", this);
    formLayout->addRow("", m_recurringCheck);

    // -- Day of Week combo (Mon=1 ... Sun=7) --
    m_dayOfWeekCombo = new QComboBox(this);
    m_dayOfWeekCombo->addItem("Monday", 1);
    m_dayOfWeekCombo->addItem("Tuesday", 2);
    m_dayOfWeekCombo->addItem("Wednesday", 3);
    m_dayOfWeekCombo->addItem("Thursday", 4);
    m_dayOfWeekCombo->addItem("Friday", 5);
    m_dayOfWeekCombo->addItem("Saturday", 6);
    m_dayOfWeekCombo->addItem("Sunday", 7);
    formLayout->addRow("Repeat Day:", m_dayOfWeekCombo);

    mainLayout->addLayout(formLayout);

    // Gombok (OK/Cancel)
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mainLayout->addWidget(m_buttonBox);

    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &AddEventDialog::onAccept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &AddEventDialog::reject);

    // Biztosítsuk, hogy az end mindig a start után legyen
    connect(m_startDateEdit, &QDateEdit::dateChanged, this, &AddEventDialog::updateEndDate);
    connect(m_startTimeEdit, &QTimeEdit::timeChanged, this, &AddEventDialog::updateEndTime);
}

AddEventDialog::~AddEventDialog()
{
}

void AddEventDialog::onAccept()
{
    // Alap validálás
    if (m_titleEdit->text().trimmed().isEmpty())
        return;

    QDateTime start(QDate(m_startDateEdit->date()), QTime(m_startTimeEdit->time()));
    QDateTime end(QDate(m_endDateEdit->date()), QTime(m_endTimeEdit->time()));
    if (start >= end)
        return;

    accept();
}

Event AddEventDialog::event() const
{
    // Alap adatok
    QString title = m_titleEdit->text();
    QDateTime start(QDate(m_startDateEdit->date()), QTime(m_startTimeEdit->time()));
    QDateTime end(QDate(m_endDateEdit->date()), QTime(m_endTimeEdit->time()));
    QString location = m_locationEdit->text();
    QString typeStr = m_typeCombo->currentText();
    EventType type = eventTypeFromString(typeStr);

    // Ismétlődés
    bool recurring = m_recurringCheck->isChecked();
    int dow = 0;
    if (recurring) {
        dow = m_dayOfWeekCombo->currentData().toInt(); // Ez az ismétlődő nap
        // Fontos: nem változtatjuk meg a start/end dátumot, így az esemény a létrehozás heti dátumán jelenik meg,
        // majd a naptár logikája a jövőbeli hetekben a 'dow' alapján ismétli az eseményt.
    }

    // Feltételezzük, hogy az Event konstruktorának paraméterei:
    // Event(QString title, QDateTime start, QDateTime end, EventType type, QString location, bool isRecurring, int repeatDayOfWeek)
    Event ev(title, start, end, type, location, recurring, dow);
    return ev;
}

void AddEventDialog::setEvent(const Event &ev)
{
    m_titleEdit->setText(ev.title());
    m_startDateEdit->setDate(ev.start().date());
    m_startTimeEdit->setTime(ev.start().time());
    m_endDateEdit->setDate(ev.end().date());
    m_endTimeEdit->setTime(ev.end().time());
    m_locationEdit->setText(ev.location());
    int idx = m_typeCombo->findText(eventTypeToString(ev.type()));
    if (idx >= 0)
        m_typeCombo->setCurrentIndex(idx);

    m_recurringCheck->setChecked(ev.isRecurring());
    if (ev.isRecurring()) {
        for (int i = 0; i < m_dayOfWeekCombo->count(); i++) {
            if (m_dayOfWeekCombo->itemData(i).toInt() == ev.repeatDayOfWeek()) {
                m_dayOfWeekCombo->setCurrentIndex(i);
                break;
            }
        }
    }
}

void AddEventDialog::updateEndDate(const QDate &date)
{
    QDate newEndDate = date;
    if (newEndDate < m_endDateEdit->date()) {
        m_endDateEdit->setDate(newEndDate);
    }
}

void AddEventDialog::updateEndTime(const QTime &time)
{
    QTime newEndTime = time.addSecs(3600);
    if (newEndTime < m_endTimeEdit->time()) {
        m_endTimeEdit->setTime(newEndTime);
    }
}
