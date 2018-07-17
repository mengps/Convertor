#include "mainwindow.h"
#include "convertor.h"
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QLineEdit>
#include <QComboBox>
#include <QRegExpValidator>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //中心部件
    QWidget *central = new QWidget(this);
    m_convertor = new Convertor(this);
    m_convertor->setFont(QFont("微软雅黑", 10));
    QBoxLayout *centralLayout = new QBoxLayout(QBoxLayout::LeftToRight, central);
    centralLayout->addWidget(m_convertor, 0, Qt::AlignCenter);
    central->setLayout(centralLayout);
    setCentralWidget(central);

    //工具栏
    QLabel *colorListLabel = new QLabel(tr("颜色表:"), this);
    m_colorComboBox = new QComboBox(this);
    m_colorComboBox->setFixedSize(170, 25);
    m_colorComboBox->setMaxCount(300);
    connect(m_convertor, &Convertor::selectBoxChanged, this, &MainWindow::fillColorComboBox);
    connect(m_colorComboBox, QOverload<const QString &>::of(&QComboBox::activated), this, [this](const QString &text)
    {
        m_fromColorEdit->setText(text);
        m_convertor->setTrack(false);
    });

    QLabel *fromColorLabel = new QLabel(tr("当前颜色:"), this);
    m_fromColorEdit = new QLineEdit("FFFFFFFF", this);
    m_fromColorEdit->setAlignment(Qt::AlignHCenter);
    m_fromColorEdit->setFixedWidth(100);
    m_fromColorEdit->setValidator(new QRegExpValidator(QRegExp("[0-9a-fA-F]{6,8}")));
    connect(m_convertor, &Convertor::currentColorChanged, m_fromColorEdit, &QLineEdit::setText);
    QLabel *toColorLabel = new QLabel(tr("目标颜色(默认透明):"), this);
    m_toColorEdit = new QLineEdit("00FFFFFF", this);
    m_toColorEdit->setAlignment(Qt::AlignHCenter);
    m_toColorEdit->setFixedWidth(100);
    m_toColorEdit->setValidator(new QRegExpValidator(QRegExp("[0-9a-fA-F]{6,8}")));

    m_toolBarWidget = new QWidget(this); 
    QGridLayout *mainLayout = new QGridLayout(m_toolBarWidget);
    mainLayout->addWidget(fromColorLabel, 0, 0, Qt::AlignHCenter);
    mainLayout->addWidget(m_fromColorEdit, 0, 1, Qt::AlignHCenter);
    mainLayout->addWidget(toColorLabel, 0, 2, Qt::AlignHCenter);
    mainLayout->addWidget(m_toColorEdit, 0, 3, Qt::AlignHCenter);
    mainLayout->addWidget(colorListLabel, 1, 0, 1, 2, Qt::AlignHCenter);
    mainLayout->addWidget(m_colorComboBox, 1, 2, 1, 2, Qt::AlignHCenter);
    m_toolBarWidget->setLayout(mainLayout);
    m_toolBar = new QToolBar(this);
    m_toolBar->addWidget(m_toolBarWidget);
    m_toolBar->setStyleSheet("font-family:微软雅黑; font-size:14px; color:rgb(200, 15, 110)");
    m_toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBar(m_toolBar);

    //菜单栏
    m_editMenu = new QMenu(tr("编辑(&E)"), this);
    m_toolMenu = new QMenu(tr("工具(&T)"), this);
    m_optionMenu = new QMenu(tr("选项(&O)"), this);
    menuBar()->addMenu(m_editMenu);
    menuBar()->addMenu(m_toolMenu);
    menuBar()->addMenu(m_optionMenu);
    createActions();

    QPalette p = palette();
    p.setBrush(QPalette::Background, QBrush(QPixmap(":/background.jpg")));
    setPalette(p);
    setAutoFillBackground(true);
    setWindowTitle(tr("颜色转换器"));
    setMinimumSize(510, 510);
    setFont(QFont("微软雅黑", 10));
    connect(m_convertor, &Convertor::finished, this, [this]()
    {
        statusBar()->showMessage(tr("转换完成！"), 1000);
    }); 
}

MainWindow::~MainWindow()
{

}

void MainWindow::createActions()
{
    QAction *convertAction = new QAction(tr("转换当前颜色"), this);
    convertAction->setShortcut(tr("Ctrl+N"));
    QAction *convertAllAction = new QAction(tr("转换选择框"), this);
    convertAllAction->setShortcut(tr("Ctrl+B"));
    QAction *convertSetAction = new QAction(tr("转换颜色表"), this);
    convertSetAction->setShortcut(tr("Ctrl+T"));
    QAction *openColorDialogAction = new QAction(tr("打开颜色板"), this);
    openColorDialogAction->setShortcut(tr("Ctrl+O"));
    QAction *saveAction = new QAction(tr("保存到本地"), this);
    saveAction->setShortcut(tr("Ctrl+S"));
    connect(convertAction, &QAction::triggered, this, [this]()
    {
        m_convertor->convertToColor(stringToRgb(m_fromColorEdit->text()), stringToRgb(m_toColorEdit->text()));
    });
    connect(convertAllAction, &QAction::triggered, this, [this]()
    {
        if (m_convertor->hasSelectBox())
            m_convertor->convertSelectBoxToColor(stringToRgb(m_toColorEdit->text()));
    });
    connect(convertSetAction, &QAction::triggered, this, [this]()
    {
        m_convertor->convertColorSet(stringToRgb(m_toColorEdit->text()));
    });

    connect(openColorDialogAction, &QAction::triggered, this, [this]()
    {
        QColor color = QColorDialog::getColor(stringToRgb(m_toColorEdit->text()));
        if (color.isValid())
            m_toColorEdit->setText(rgbToString(color.rgba()));
    });
    connect(saveAction, &QAction::triggered, this, [this]()
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("保存图片"), QString(), tr("图片 (*.png)"));
        if (!fileName.isEmpty())
            m_convertor->pixmap()->save(fileName, "PNG");
    });
    m_editMenu->addAction(convertAction);
    m_editMenu->addAction(convertSetAction);
    m_editMenu->addAction(convertAllAction);
    m_editMenu->addSeparator();
    m_editMenu->addAction(openColorDialogAction);
    m_editMenu->addAction(saveAction);

    QAction *linesAction = new QAction(QIcon(":/lines.png"), tr("线形"), this);
    QAction *rectAction = new QAction(QIcon(":/rect.png"), tr("矩形"), this);
    QAction *ellipseAction = new QAction(QIcon(":/ellipse.png"), tr("椭圆形"), this);
    connect(linesAction, &QAction::triggered, this, [this]()
    {
        m_convertor->setShape(ShapeController::Lines);
    });
    connect(rectAction, &QAction::triggered, this, [this]()
    {
        m_convertor->setShape(ShapeController::Rect);
    });
    connect(ellipseAction, &QAction::triggered, this, [this]()
    {
        m_convertor->setShape(ShapeController::Ellipse);
    });
    m_toolMenu->addAction(linesAction);
    m_toolMenu->addAction(rectAction);
    m_toolMenu->addAction(ellipseAction);

    QAction *trackAction = new QAction(tr("鼠标跟踪：开"), this);
    connect(m_convertor, &Convertor::trackChanged, this, [=]()
    {
        trackAction->setText(tr("鼠标跟踪：%1").arg(m_convertor->track() ? "开" : "关"));
    });
    m_optionMenu->addAction(trackAction);
}

void MainWindow::fillColorComboBox()
{
    m_colorComboBox->clear();
    for (auto it : m_convertor->getColorSet())
    {
        QPixmap pix_color(70,20);
        pix_color.fill(stringToRgb(it));
        m_colorComboBox->addItem(QIcon(pix_color), it);
        m_colorComboBox->setIconSize(QSize(70,20));
        m_colorComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    m_toolBarWidget->setFixedWidth(m_toolBar->width());
}

