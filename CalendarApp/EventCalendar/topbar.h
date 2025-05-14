#ifndef TOPBAR_H
#define TOPBAR_H

#include <QWidget>
#include <QDate>

class QPushButton;
class QLabel;

class TopBar : public QWidget
{
    Q_OBJECT
public:
    explicit TopBar(QWidget *parent = nullptr);

signals:
    void dateChanged(const QDate &date);
    void addEventClicked();

private slots:
    void onTodayClicked();
    void onPrevWeekClicked();
    void onNextWeekClicked();
    void onAddEventClicked();

private:
    QPushButton* m_btnPrev;
    QPushButton* m_btnNext;
    QPushButton* m_btnToday;
    QPushButton* m_btnAddEvent;  // Kék + gomb
    QLabel* m_dateLabel;

    QDate m_currentDate;

    void updateDateLabel();
};

#endif // TOPBAR_H
