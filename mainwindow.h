#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QMenu;
class QLineEdit;
class QComboBox;
class Conversion;
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void createActions();

private:
    QMenu *menu;
    QLineEdit *m_fromColorEdit;
    QLineEdit *m_toColorEdit;
    QComboBox *m_colorComboBox;
    Conversion *m_conversion;
};

#endif // MAINWINDOW_H
