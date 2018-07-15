#include "convertor.h"
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
    m_hasSelectedRect = false;
    setAlignment(Qt::AlignCenter);
    setAcceptDrops(true);
    setMouseTracking(true);
    setStyleSheet("border:1px solid #000; background-color:transparent;");
    clear();
}

Conversion::~Conversion()
{

}

void Conversion::clear()
{
    setText(tr("<拖动图像到此>"));
    setFixedSize(200, 200);
}

void Conversion::clearSelectedRect()
{
    m_hasSelectedRect = false;
    m_selectedRect.setRect(0, 0, 0, 0);
    update();
}

QRect Conversion::normalizeRect()
{
    QRect rect = m_selectedRect.normalized();
    int x = 0;
    int y = 0;
    if (rect.x() >= 0 && rect.x() <= m_image.width())
        x = rect.x();
    if (rect.y() >= 0 && rect.y() <= m_image.height())
        y = rect.y();
    int w = x + rect.width();
    int h = y + rect.height();
    if (w > m_image.width())
        w = m_image.width();
    if (h > m_image.height())
        h = m_image.height();

    return QRect(x, y, w, h);
}

void Conversion::setImage(const QImage &image)
{
    m_image = image.convertToFormat(QImage::Format_RGBA8888);
    setPixmap(QPixmap::fromImage(m_image));
    setFixedSize(m_image.size());
}

bool Conversion::convertInSetColor(QRgb to_argb)
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
                    if (m_rectColorSet.contains(argb))
                        image.setPixel(x, y, to_argb);
                }
            }
            setImage(image);
            clearSelectedRect();
        });
        return true;
    }
    else return false;
}

bool Conversion::convertSelectedRectToColor(QRgb to_argb)
{
    if (!m_image.isNull())
    {
        QImage image = m_image;
        QRect rect = normalizeRect();
        for (int i = rect.x(); i < rect.width(); i++)
        {
            for (int j = rect.y(); j < rect.height(); j++)
            {
                image.setPixel(i, j, to_argb);
            }
        }
        setImage(image);
        clearSelectedRect();
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
        clearSelectedRect();
        return true;
    }
    else return false;
}

void Conversion::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    QRect rect = m_selectedRect.normalized();
    QPainter painter(this);
    painter.setBrush(QBrush(QColor(144, 144, 144, 58)));
    painter.drawRect(rect);
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
    m_hasSelectedRect = false;
    m_selectedRect.setRect(0, 0, 0, 0);
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasImage()) {
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
    m_rectColorSet.clear();
    emit selectedRectChanged();
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

    if (m_hasSelectedRect && (event->buttons() & Qt::LeftButton))
    {
        m_selectedRect.setWidth(event->x() - m_selectedRect.x());
        m_selectedRect.setHeight(event->y() - m_selectedRect.y());

        m_rectColorSet.clear();    //重新装入数据
        QRect rect = normalizeRect();
        for (int i = rect.x(); i < rect.width(); i++)
        {
            for (int j = rect.y(); j < rect.height(); j++)
            {
                m_rectColorSet.insert(rgbToString(m_image.pixel(i, j)));
                if (m_rectColorSet.count() >= 300)  //最多保存300种颜色，如需更多请自行更改
                    break;
            }
        }

        emit selectedRectChanged();
        update();
    }
}

void Conversion::mousePressEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton))
    {
        m_hasSelectedRect = true;
        m_selectedRect.setX(event->x());
        m_selectedRect.setY(event->y());
    }
    else if (event->buttons() & Qt::RightButton)
    {
        m_track = !m_track;
        m_hasSelectedRect = false;
        m_selectedRect.setRect(0, 0, 0, 0);
        update();
    }
}
