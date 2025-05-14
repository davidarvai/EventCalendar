#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class TopBar;
class LeftSideBar;
class WeekView;
class EventModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    TopBar* m_topBar;
    LeftSideBar* m_leftSideBar;
    WeekView* m_weekView;
    EventModel* m_eventModel;

    void setupUi();
    void setupConnections();
};

#endif // MAINWINDOW_H
