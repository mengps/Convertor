﻿#include "convertor.h"
#include <QToolTip>
#include <QPainter>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QtConcurrent>

extern QString rgbToString(QRgb rgb)
{
    QString argb = QString::number(rgb, 16);
    if (!(rgb & 0xFF000000))  //alpha为00
        argb.insert(0, "00");
    else if (!(rgb & 0xF0000000))  //alpha为0*
        argb.insert(0, "0");

    return argb.toUpper();
}

extern QRgb stringToRgb(const QString &argb)
{
    QString text = argb;
    if (text.length() == 6)         //为rgb
       text.insert(0, "0xFF");
    else if (text.length() == 7)    //补齐
        text.insert(0, "0xF");
    else text.insert(0, "0x");

    return QRgb(text.toUInt(nullptr, 16));
}

Conversion::Conversion(QWidget *parent)
    : QLabel(parent)
{
    m_track = true;
    m_hasSelectBox = false;
    setAlignment(Qt::AlignCenter);
    setAcceptDrops(true);
    setMouseTracking(true);
    setStyleSheet("border:1px solid #222");
    clear();
}

Conversion::~Conversion()
{

}

void Conversion::clear()
{
    setText(tr("<拖动图像到此>"));
    setFixedSize(240, 240);
    m_shapeController.setFixedSize(size());
}

void Conversion::setImage(const QImage &image)
{
    m_image = image.convertToFormat(QImage::Format_RGBA8888);
    setPixmap(QPixmap::fromImage(m_image));
    setFixedSize(m_image.size());
    m_shapeController.setFixedSize(m_image.size());
}

bool Conversion::convertColorSet(QRgb to_argb)
{
    if (!m_image.isNull())
    {
        QtConcurrent::run([=]() //开启一个新的线程进行处理
        {
            QImage image = m_image;
            for (int x = 0; x < image.width(); x++)
            {
                for (int y = 0; y < image.height(); y++)
                {
                    QString argb = rgbToString(image.pixel(x, y));
                    if (m_colorSet.contains(argb))
                        image.setPixel(x, y, to_argb);
                }
            }
            setImage(image);
            m_shapeController.clear();
        });
        return true;
    }
    else return false;
}

bool Conversion::convertSelectBoxToColor(QRgb to_argb)
{
    if (!m_image.isNull())
    {
        QImage image = m_image;
        QVector<QPoint> points = m_shapeController.pointsInShape();
        for (auto it : points)
            image.setPixel(it, to_argb);
        setImage(image);
        return true;
    }
    else return false;
}

bool Conversion::convertToColor(QRgb from_argb, QRgb to_argb)
{
    if (!m_image.isNull())
    {
        QImage image = m_image;
        for (int x = 0; x < image.width(); x++)
        {
            for (int y = 0; y < image.height(); y++)
            {
                if (image.pixel(x, y) == from_argb)
                    image.setPixel(x, y, to_argb);
            }
        }
        setImage(image);
        m_shapeController.clear();
        return true;
    }
    else return false;
}

void Conversion::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    m_shapeController.drawShape(this);
}

void Conversion::dragEnterEvent(QDragEnterEvent *event)
{
    setText(tr("<松开鼠标放入!>"));
    event->acceptProposedAction();
}

void Conversion::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void Conversion::dragLeaveEvent(QDragLeaveEvent *event)
{
    clear();
    event->accept();
}

void Conversion::dropEvent(QDropEvent *event)
{
    m_hasSelectBox = false;
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasImage())
    {
        setImage(qvariant_cast<QImage>(mimeData->imageData()));
    }
    else if(mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        QImage image = QImage(urlList.at(0).path().mid(1)); //路径前会有一个'/'，去掉
        if (!image.isNull())
            setImage(image);
        else
            setText(tr("<非图像文件!>"));
    }
    else clear();
    m_colorSet.clear();
    emit selectBoxChanged();
}

void Conversion::mouseMoveEvent(QMouseEvent *event)
{
    if (m_track)
    {
        if (m_image.valid(event->x(), event->y()))
        {
            QString argb = rgbToString(m_image.pixel(event->pos()));
            QToolTip::showText(event->globalPos(), "  ARGB值: " + argb + "\n右键(开启/关闭)鼠标跟踪");
            emit currentColorChanged(argb);
        }
    }

    if (m_hasSelectBox && (event->buttons() & Qt::LeftButton))
    {
        m_shapeController.setX2(event->x());
        m_shapeController.setY2(event->y());

        if (!m_image.isNull())
        {
            QVector<QPoint> points = m_shapeController.pointsInShape();
            for (auto it : points)
            {
                m_colorSet.insert(rgbToString(m_image.pixel(it)));
                if (m_colorSet.count() >= 1000)  //最多保存1000种颜色，如需更多请自行更改
                    break;
            }
        }
        emit selectBoxChanged();
        update();
    }
}

void Conversion::mousePressEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton))
    {
        m_hasSelectBox = true;
        m_colorSet.clear();    //重新装入数据
        m_shapeController.clear();
        m_shapeController.setX1(event->x());
        m_shapeController.setY1(event->y());
    }
    else if (event->buttons() & Qt::RightButton)
    {
        m_track = !m_track;
        m_hasSelectBox = false;
        m_shapeController.clear();
        update();
    }
}
