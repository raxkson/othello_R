#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QGridLayout>
#include <QDebug>
#include <QDateTime>
#include <QElapsedTimer>
#include <QTimer>
#include <QTime>
#include "Disc.h"
#include "OthelloState.h"
#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <array>
#include <sstream>
#include <unistd.h>

#include "othello.h"
#define DiscSize 8

typedef QVector< QVector<Disc*> > ChessDiscs;
typedef QVector< QVector< QVector<Type> > > BACKUP;
typedef enum {
    HUMAN,
    COMP,
    NET
} PlayerType;

namespace Ui {
class MainWindow;
}
using namespace std;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    othello game;
    QTimer * timer;
    QTime * TimeRecord;
    bool isStart;

    ChessDiscs Discs;
    char Disc_array[65][65];
    int NumberOfDiscCanEat(int, int, int, int);
    int NumberOfDiscCanEatTotal(int, int);
    void DropThisDisc(int, int);
    void EatDiscs(int, int, int, int);
    Type Player;
    Type Enemy;
    OthelloState state;
    int NumberOfBlack;
    int NumberOfWhite;
    int totalMove;
    void Refresh();

private slots:
    void getDropDisc(int, int );
    void Restart();
    void AI();
    void Connect();
    void CON();
    void updateTime();
    void delay(int);
    void Start();
};

#endif // MAINWINDOW_H
