#ifndef MYAVAILABILITY_H
#define MYAVAILABILITY_H

#include <QWidget>
#include <QMap>
#include <QStringList>

class QLabel;
class QPushButton;
class QCheckBox;
class QVBoxLayout;
class QIcon;


class MyAvailability : public QWidget
{
    Q_OBJECT
public:
    explicit MyAvailability(QWidget *parent = nullptr);

signals:
    void availabilityChanged(const QStringList &activeTypes);

private slots:
    void toggleMenu();
    void onCheckBoxToggled(bool checked);

private:
    QLabel* m_label;
    QPushButton* m_arrowButton;
    QWidget* m_dropDownWidget;
    QVBoxLayout* m_dropDownLayout;
    QMap<QString, QCheckBox*> m_checkBoxes;
    bool m_menuVisible;

    QIcon createColoredIcon(const QColor &color, bool checked);
};

#endif // MYAVAILABILITY_H
