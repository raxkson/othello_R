#include "Disc.h"

Disc::Disc(QWidget *parent) :
    QPushButton(parent)
{
    type = Empty;
}

void Disc::setIndex(int i, int j)
{
    row = i;
    column = j;
}

void Disc::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit DropDisc(row, column);
    }
}
