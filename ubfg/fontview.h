#ifndef FONTVIEW_H
#define FONTVIEW_H

#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QImage>

class FontView : public QWidget
{
Q_OBJECT
public:
    FontView(QWidget *parent);
protected:
    void paintEvent(QPaintEvent *event);
private:
    QPixmap texture;
    int scale;
private Q_SLOTS:
    void updatePixmap(const QImage &image);
    void rescale(int);
};

#endif // FONTVIEW_H
