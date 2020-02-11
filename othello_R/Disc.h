#ifndef Disc_H
#define Disc_H

#include <QPushButton>
#include <QMouseEvent>

enum Type{
    White,
    Black,
    Empty
};

class Disc : public QPushButton
{
    Q_OBJECT
public:
    Disc(QWidget *parent = 0);
    void setIndex(int, int);
    Type type;

private:
    int row;
    int column;

protected:
    virtual void mousePressEvent(QMouseEvent* event);

signals:
    void DropDisc(int, int);
};

#endif // Disc_H
