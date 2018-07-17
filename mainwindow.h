#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QMenu;
class QLineEdit;
class QComboBox;
class Convertor;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void createActions();

public slots:
    void fillColorComboBox();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QMenu *m_editMenu;
    QMenu *m_toolMenu;
    QMenu *m_optionMenu;
    QToolBar *m_toolBar;
    QWidget *m_toolBarWidget;
    QLineEdit *m_fromColorEdit;
    QLineEdit *m_toColorEdit;
    QComboBox *m_colorComboBox;
    Convertor *m_convertor;
};

#endif // MAINWINDOW_H
