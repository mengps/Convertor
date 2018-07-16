#include "mainwindow.h"
#include "convertor.h"
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QRegExpValidator>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    QLabel *fromColorLabel = new QLabel(tr("要转换的颜色:"), this);
    m_fromColorEdit = new QLineEdit("FFFFFFFF", this);
    m_fromColorEdit->setAlignment(Qt::AlignHCenter);
    m_fromColorEdit->setFixedWidth(100);
    m_fromColorEdit->setValidator(new QRegExpValidator(QRegExp("[0-9a-fA-F]{6,8}")));
    QLabel *toColorLabel = new QLabel(tr("目标颜色(默认为透明):"), this);
    m_toColorEdit = new QLineEdit("00FFFFFF", this);
    m_toColorEdit->setAlignment(Qt::AlignHCenter);
    m_toColorEdit->setFixedWidth(100);
    m_toColorEdit->setValidator(new QRegExpValidator(QRegExp("[0-9a-fA-F]{6,8}")));

    QPushButton *openColorDialog = new QPushButton(tr("+++"), this);
    QPushButton *convertButton = new QPushButton(tr("转换此颜色"), this);
    QPushButton *convertSetButton = new QPushButton(tr("转换所有颜色"), this);
    QPushButton *convertAllButton = new QPushButton(tr("转换选择框"), this);
    QPushButton *saveButton = new QPushButton(tr("保存到本地"), this);
    QLabel *colorListLabel = new QLabel(tr("选择框内的颜色:"), this);
    m_colorComboBox = new QComboBox(this);
    m_colorComboBox->setMaxCount(300);
    m_conversion = new Conversion(this);
    connect(m_conversion, &Conversion::currentColorChanged, m_fromColorEdit, &QLineEdit::setText);
    connect(openColorDialog, &QPushButton::clicked, this, [this]()
    {
        QColor color = QColorDialog::getColor(stringToRgb(m_toColorEdit->text()));
        if (color.isValid())
            m_toColorEdit->setText(rgbToString(color.rgba()));
    });
    connect(convertButton, &QPushButton::clicked, m_conversion, [this]()
    {
        m_conversion->convertToColor(stringToRgb(m_fromColorEdit->text()), stringToRgb(m_toColorEdit->text()));
    });
    connect(convertSetButton, &QPushButton::clicked, this, [this]()
    {
        m_conversion->convertColorSet(stringToRgb(m_toColorEdit->text()));
    });
    connect(convertAllButton, &QPushButton::clicked, this, [this]()
    {
        if (m_conversion->hasSelectBox())
            m_conversion->convertSelectBoxToColor(stringToRgb(m_toColorEdit->text()));
    });
    connect(saveButton, &QPushButton::clicked, this, [this]()
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("保存图片"), QString(), tr("图片 (*.png)"));
        if (!fileName.isEmpty())
            m_conversion->pixmap()->save(fileName, "PNG");
    });
    connect(m_conversion, &Conversion::selectBoxChanged, this, [this]()
    {
        m_colorComboBox->clear();
        for (auto it : m_conversion->getRectColorSet())
        {
            QPixmap pix_color(70,20);
            pix_color.fill(stringToRgb(it));
            m_colorComboBox->addItem(QIcon(pix_color), it);
            m_colorComboBox->setIconSize(QSize(70,20));
            m_colorComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        }
    });
    connect(m_colorComboBox, QOverload<const QString &>::of(&QComboBox::activated), this, [this](const QString &text)
    {
        m_fromColorEdit->setText(text);
        m_conversion->stopTrack();
    });

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(toColorLabel, 0, 0, Qt::AlignHCenter);
    mainLayout->addWidget(m_toColorEdit, 0, 1, Qt::AlignHCenter);
    mainLayout->addWidget(openColorDialog, 0, 2, Qt::AlignHCenter);
    mainLayout->addWidget(colorListLabel, 1, 0, Qt::AlignHCenter);
    mainLayout->addWidget(m_colorComboBox, 1, 1, Qt::AlignHCenter);
    mainLayout->addWidget(convertSetButton, 1, 2, Qt::AlignHCenter);
    mainLayout->addWidget(fromColorLabel, 2, 0, Qt::AlignHCenter);
    mainLayout->addWidget(m_fromColorEdit, 2, 1, Qt::AlignHCenter);
    mainLayout->addWidget(convertButton, 2, 2, Qt::AlignHCenter);
    mainLayout->addWidget(convertAllButton, 3, 0, 1, 2, Qt::AlignHCenter);
    mainLayout->addWidget(saveButton, 3, 2, Qt::AlignHCenter);
    mainLayout->addWidget(m_conversion, 4, 0, -1, 3, Qt::AlignCenter);

    setLayout(mainLayout);
    setWindowTitle(tr("颜色转换器"));
    setMinimumSize(510, 510);
    setAutoFillBackground(true);
    QPalette p = palette();
    p.setBrush(QPalette::Background, QBrush(QPixmap(":/background.jpg")));
    setPalette(p);
    m_conversion->setFont(QFont("微软雅黑", 10));
    setFont(QFont("微软雅黑", 10));
}

MainWindow::~MainWindow()
{

}

void MainWindow::createActions()
{

}

