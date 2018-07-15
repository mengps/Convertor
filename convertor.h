#ifndef CONVERSION_H
#define CONVERSION_H

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
    void clearSelectedRect();
    QRect normalizeRect();

    void setImage(const QImage &image); 
    bool convertInSetColor(QRgb to_argb);               //转换combobox内的颜色
    bool convertSelectedRectToColor(QRgb to_argb);      //转换矩形框内的颜色
    bool convertToColor(QRgb from_argb, QRgb to_argb);  //将from_argb转换到to_argb

    bool hasSelectedRect() { return m_hasSelectedRect; }
    void stopTrack() { m_track = false; }
    QSet<QString> getRectColorSet() const { return m_rectColorSet; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void selectedRectChanged();
    void currentColorChanged(const QString &argb);

private:
    bool m_track;
    bool m_hasSelectedRect;
    QImage m_image;
    QRect m_selectedRect;
    QSet<QString> m_rectColorSet;
};

#endif // CONVERSION_H
