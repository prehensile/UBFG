#include "fontview.h"
#include <QMouseEvent>
#include <QToolTip>


FontView::FontView(QWidget *parent)
{
    scale = 1;
    setMouseTracking(true);
}


void FontView::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::magenta);
    this->setMinimumSize(texture.width()*scale, texture.height()*scale);
    painter.drawPixmap(0, 0, this->width(), this->height(), texture);
}

void FontView::mouseMoveEvent(QMouseEvent* event)
{
    QToolTip::showText(event->globalPos(),
                       //  In most scenarios you will have to change these for
                       //  the coordinate system you are working in.
                       QString::number( event->pos().x()/scale ) + ", " +
                       QString::number( event->pos().y()/scale ),
                       this, rect() );
    QWidget::mouseMoveEvent(event);  // Or whatever the base class is.
}


void FontView::updatePixmap(const QImage &image)
{
    texture = QPixmap::fromImage(image);
    this->setMinimumSize(texture.width(), texture.height());
    update();
}

void FontView::rescale(int index)
{
    switch(index)
    {
        case 3:
            scale = 8;
            break;
        case 2:
            scale = 4;
            break;
        case 1:
            scale = 2;
            break;
        case 0:
            scale = 1;
            break;
    }
    this->update();
}
