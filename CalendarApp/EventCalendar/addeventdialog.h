#ifndef ADDEVENTDIALOG_H
#define ADDEVENTDIALOG_H

#include <QDialog>
#include <QDate>
#include <QTime>
#include "event.h"

class QLineEdit;
class QDateEdit;
class QTimeEdit;
class QComboBox;
class QDialogButtonBox;
class QCheckBox;

class AddEventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEventDialog(QWidget *parent = nullptr);
    ~AddEventDialog();

    Event event() const;
    void setEvent(const Event &ev);

private slots:
    void onAccept();
    void updateEndDate(const QDate &date);
    void updateEndTime(const QTime &time);

private:
    QLineEdit *m_titleEdit;
    QDateEdit *m_startDateEdit;
    QTimeEdit *m_startTimeEdit;
    QDateEdit *m_endDateEdit;
    QTimeEdit *m_endTimeEdit;
    QLineEdit *m_locationEdit;
    QComboBox *m_typeCombo;
    QDialogButtonBox *m_buttonBox;
    QCheckBox* m_recurringCheck;
    QComboBox* m_dayOfWeekCombo;
};

#endif // ADDEVENTDIALOG_H
