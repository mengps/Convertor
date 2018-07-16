#ifndef SHAPECONTROLLER_H
#define SHAPECONTROLLER_H
#include <QRectF>

class QPaintDevice;
class ShapeController
{
public:
    enum Shape
    {
        Lines,
        Rect,
        Ellipse
    };

    ShapeController();
    ~ShapeController();

    void clear();
    QRectF normalized();
    QVector<QPoint> pointsInShape();

    qreal x1() const { return m_rect.x(); }
    qreal y1() const { return m_rect.y(); }
    void setX1(qreal x1);
    void setY1(qreal y1);
    void setX2(qreal x2);
    void setY2(qreal y2);

    void setShape(Shape shape) { m_shape = shape; }
    void setFixedSize(QSizeF size) { m_fixedSize = size; }
    void drawShape(QPaintDevice *device);

private:
    bool pointInEllipse(qreal x, qreal y);
    void drawRect(QPaintDevice *device);
    void drawEllipse(QPaintDevice *device);

private:
    QSizeF m_fixedSize;
    QRectF m_rect;
    Shape m_shape;
};

#endif // SHAPECONTROLLER_H
