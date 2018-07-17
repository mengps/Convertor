#include "shapecontroller.h"
#include <QtMath>
#include <QPainter>
#include <QPaintDevice>

ShapeController::ShapeController()
{
    m_shape = ShapeController::Rect;
}

ShapeController::~ShapeController()
{

}

void ShapeController::clear()
{
    m_rect.setRect(0, 0, 0, 0);
}

void ShapeController::setX1(qreal x1)
{
    if (x1 >= 0 && x1 <= m_fixedSize.width())
        m_rect.setX(x1);
}

void ShapeController::setY1(qreal y1)
{
    if (y1 >= 0 && y1 <= m_fixedSize.height())
        m_rect.setY(y1);
}

void ShapeController::setX2(qreal x2)
{
    m_rect.setWidth(x2 - m_rect.x());
}

void ShapeController::setY2(qreal y2)
{
    m_rect.setHeight(y2 - m_rect.y());
}

bool ShapeController::pointInEllipse(qreal x, qreal y)
{
    qreal a = m_rect.width() / 2;
    qreal b = m_rect.height() / 2;
    qreal xc = m_rect.x() + a;
    qreal yc = m_rect.y() + b;
    qreal result = qPow(x - xc, 2) / qPow(a, 2) + qPow(y - yc, 2) / qPow(b, 2);

    return result <= 1.0;
}

QVector<QPoint> ShapeController::pointsInShape()
{
    QVector<QPoint> points;
    QRectF rect = normalized();
    switch(m_shape)
    {
    case Lines:
        break;
    case Rect:
        for (int x = rect.x(); x < rect.width(); x++)
        {
            for (int y = rect.y(); y < rect.height(); y++)
                points.append(QPoint(x, y));
        }
        break;
    case Ellipse:
        for (int x = rect.x(); x < rect.width(); x++)
        {
            for (int y = rect.y(); y < rect.height(); y++)
                if (pointInEllipse(x, y))
                    points.append(QPoint(x, y));
        }
        break;
    default:
        break;
    }

    return points;
}

void ShapeController::drawShape(QPaintDevice *device)
{
    switch(m_shape)
    {
    case Lines:
        break;
    case Rect:
        drawRect(device);
        break;
    case Ellipse:
        drawEllipse(device);
        break;
    default:
        break;
    }
}

QRectF ShapeController::normalized()
{
    QRectF rect = m_rect.normalized();
    qreal x = 0;
    qreal y = 0;
    if (rect.x() >= 0 && rect.x() <= m_fixedSize.width())
        x = rect.x();
    if (rect.y() >= 0 && rect.y() <= m_fixedSize.height())
        y = rect.y();
    qreal w = rect.x() + rect.width();
    qreal h = rect.y() + rect.height();
    if (w > m_fixedSize.width())
        w = m_fixedSize.width();
    if (h > m_fixedSize.height())
        h = m_fixedSize.height();

    return QRectF(x, y, w, h);
}

void ShapeController::drawRect(QPaintDevice *device)
{
    QPainter painter(device);
    painter.setBrush(QBrush(QColor(255, 0, 0, 38)));
    painter.drawRect(m_rect.normalized());
}

void ShapeController::drawEllipse(QPaintDevice *device)
{
    QPainter painter(device);
    painter.setBrush(QBrush(QColor(255, 0, 0, 38)));
    painter.drawEllipse(m_rect.normalized());
}

