#include "myavailability.h"
#include "eventtype.h"

#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QDebug>

MyAvailability::MyAvailability(QWidget *parent)
    : QWidget(parent),
    m_dropDownWidget(nullptr),
    m_dropDownLayout(nullptr),
    m_menuVisible(false)
{
    // Fő layout a widgetben
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Felső sor: bal oldalon a lefelé mutató háromszög gomb, majd a "My Availability:" felirat
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->setContentsMargins(0, 0, 0, 0);

    m_arrowButton = new QPushButton(this);
    m_arrowButton->setText(QChar(0x25BC)); // ▼
    m_arrowButton->setFlat(true);
    m_arrowButton->setStyleSheet("border: none; color: black;");
    m_arrowButton->setCursor(Qt::PointingHandCursor);
    m_arrowButton->setFixedSize(10, 10);
    topLayout->addWidget(m_arrowButton);

    m_label = new QLabel("My Availability", this);
    QFont font = m_label->font();
    font.setPointSize(10);
    font.setBold(true);
    m_label->setFont(font);
    topLayout->addWidget(m_label);

    topLayout->addStretch();
    mainLayout->addLayout(topLayout);

    // Lenyíló widget létrehozása (alapból rejtett)
    m_dropDownWidget = new QWidget(this);
    // Módosítás: eltávolítjuk a keretet (rácsos hatást)
    m_dropDownWidget->setStyleSheet("background-color: transparent; border: none;");
    m_dropDownLayout = new QVBoxLayout(m_dropDownWidget);
    m_dropDownLayout->setContentsMargins(4, 4, 4, 4);

    // Példa opciók
    QStringList types;
    types << "Meeting" << "Workout" << "Personal";

    // Checkboxok létrehozása
    for (const QString &type : types) {
        QCheckBox* cb = new QCheckBox(type, m_dropDownWidget);
        // Az alapértelmezett indikátor eltüntetése
        cb->setStyleSheet("QCheckBox::indicator { width: 0px; }");

        // Kezdeti ikon: fehér belső rész, színes, vastag keret (a pipálatlan állapotban)
        QColor color = eventTypeColor(eventTypeFromString(type));
        cb->setIcon(createColoredIcon(color, false));

        connect(cb, &QCheckBox::toggled, this, &MyAvailability::onCheckBoxToggled);
        m_checkBoxes[type] = cb;
        m_dropDownLayout->addWidget(cb);
    }
    m_dropDownWidget->setVisible(false);
    mainLayout->addWidget(m_dropDownWidget);

    setLayout(mainLayout);

    connect(m_arrowButton, &QPushButton::clicked, this, &MyAvailability::toggleMenu);
}

void MyAvailability::toggleMenu()
{
    m_menuVisible = !m_menuVisible;
    m_dropDownWidget->setVisible(m_menuVisible);
}

void MyAvailability::onCheckBoxToggled(bool /*checked*/)
{
    QCheckBox* senderCb = qobject_cast<QCheckBox*>(sender());
    if (!senderCb)
        return;

    QString typeText = senderCb->text();
    EventType et = eventTypeFromString(typeText);
    QColor color = eventTypeColor(et);
    senderCb->setIcon(createColoredIcon(color, senderCb->isChecked()));

    QStringList active;
    for (auto it = m_checkBoxes.begin(); it != m_checkBoxes.end(); ++it) {
        if (it.value()->isChecked()) {
            active << it.key();
        }
    }
    emit availabilityChanged(active);
}

QIcon MyAvailability::createColoredIcon(const QColor &color, bool checked)
{
    const int iconSize = 16;
    QPixmap pix(iconSize, iconSize);
    pix.fill(Qt::transparent);

    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);

    // Meghatározzuk a lekerekítés sugarát (például 3 pixel)
    const qreal radius = 3.0;

    if (checked) {
        // Pipált állapot: az egész négyzet kitöltődik az adott színnel, lekerekített sarkokkal
        painter.setBrush(color);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(0, 0, iconSize - 1, iconSize - 1, radius, radius);

        // Pipa rajzolása
        QPen pen(Qt::black, 2);
        painter.setPen(pen);
        painter.drawLine(iconSize / 4, iconSize / 2,
                         iconSize / 2, iconSize - iconSize / 4);
        painter.drawLine(iconSize / 2, iconSize - iconSize / 4,
                         iconSize - iconSize / 4, iconSize / 4);
    } else {
        // Nem pipált: fehér belső rész, vastag színes keret, lekerekített sarkokkal
        painter.setBrush(Qt::white);
        QPen pen(color, 2);
        painter.setPen(pen);
        painter.drawRoundedRect(0, 0, iconSize - 1, iconSize - 1, radius, radius);
    }

    painter.end();
    return QIcon(pix);
}
