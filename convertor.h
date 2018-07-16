#ifndef CONVERSION_H
#define CONVERSION_H

#include "shapecontroller.h"
#include <QSet>
#include <QLabel>

QString rgbToString(QRgb rgb);
QRgb stringToRgb(const QString &argb);

class Conversion : public QLabel
{
    Q_OBJECT

public:
    Conversion(QWidget *parent = 0);
    ~Conversion();

    void clear();

    void setImage(const QImage &image); 
    void setShape(ShapeController::Shape shape) { m_shapeController.setShape(shape); }
    bool convertColorSet(QRgb to_argb);                    //转换combobox内的颜色
    bool convertSelectBoxToColor(QRgb to_argb);            //转换选择框框内的颜色
    bool convertToColor(QRgb from_argb, QRgb to_argb);     //将from_argb转换到to_argb

    bool hasSelectBox() { return m_hasSelectBox; }
    void stopTrack() { m_track = false; }
    QSet<QString> getRectColorSet() const { return m_colorSet; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void selectBoxChanged();
    void currentColorChanged(const QString &argb);

private:
    bool m_track;
    bool m_hasSelectBox;
    QImage m_image;
    QSet<QString> m_colorSet;
    ShapeController m_shapeController;
};

#endif // CONVERSION_H
