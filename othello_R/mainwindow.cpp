#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ai.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    srand(0);
    ui->setupUi(this);
    this->setWindowTitle("Othello_R");

    isStart = false;
    timer = new QTimer;
    TimeRecord = new QTime(0, 0, 0);

    ui->Timer->setDigitCount(8);
    ui->Timer->setSegmentStyle(QLCDNumber::Flat);
    ui->Timer->display(TimeRecord->toString("mm:ss"));
    connect(timer,SIGNAL(timeout()),this,SLOT(updateTime()));

    ui->BlackAI->addItem("human", HUMAN);
    ui->BlackAI->addItem("ai", COMP);
    ui->BlackAI->addItem("net", NET);

    ui->WhiteAI->addItem("human", HUMAN);
    ui->WhiteAI->addItem("ai", COMP);
    ui->WhiteAI->addItem("net", NET);


    /* Discs[DiscSize][DiscSize] */
    Discs.resize(DiscSize);
    for(int i = 0; i < DiscSize; i++){
        Discs[i].resize(DiscSize);
    }

    QGridLayout* ChessBoardLayout = new QGridLayout;

    ChessBoardLayout->setMargin(0);
    ChessBoardLayout->setSpacing(0);

    /* Construct Discs[DiscSize][DiscSize] */
    for(int i = 0; i < DiscSize; i++)
    {
        for(int j = 0; j < DiscSize; j++)
        {
            Discs[i][j] = new Disc();
            Discs[i][j]->setIndex(i, j);
            Discs[i][j]->setFixedSize(40, 40);
            Discs[i][j]->setIconSize(Discs[i][j]->size());
            connect(Discs[i][j], SIGNAL(DropDisc(int,int)), this, SLOT(getDropDisc(int,int)));
            ChessBoardLayout->addWidget(Discs[i][j], i*40, j*40);
        }
    }

    connect(ui->Restart, SIGNAL(clicked(bool)), this, SLOT(Restart()));
    connect(ui->AI, SIGNAL(clicked(bool)), this, SLOT(Start()));
    connect(ui->Connect, SIGNAL(clicked(bool)),this, SLOT(Connect()));
    //connect(ui->AI, SIGNAL(clicked(bool)), this, SLOT(AI()));


    ui->BlackDiscImage->setPixmap(QPixmap(":/Image/BlackChess.png").scaled(40, 40, Qt::KeepAspectRatio));
    ui->WhiteDiscImage->setPixmap(QPixmap(":/Image/WhiteChess.png").scaled(40, 40, Qt::KeepAspectRatio));
    ui->ChessBoardLabel->setLayout(ChessBoardLayout);

    /* The Game Begin */
    Restart();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::Connect(){
    QString ip = ui->serverip->text();
    QString port = ui->serverport->text();
    game = othello(ip.toStdString(), port.toInt());
    Start();
}
void MainWindow::Start(){
    if(NET == (PlayerType)(ui->BlackAI->currentIndex())){
        CON();
    }
    else if ( NET == (PlayerType)(ui->WhiteAI->currentIndex())){
        CON();
    } else {
        AI();
    }
}

void MainWindow::Restart()
{
    for(int i = 0; i < DiscSize; i++)
    {
        for(int j = 0; j < DiscSize; j++)
        {
            Discs[i][j]->setIcon(QIcon());
            Discs[i][j]->type = Empty;
        }
    }

    state = OthelloState(DiscSize);

    Discs[DiscSize/2-1][DiscSize/2-1]->setIcon(QIcon(QPixmap(":/Image/WhiteChess.png"))); Discs[DiscSize/2-1][DiscSize/2-1]->type = White;
    Discs[DiscSize/2-0][DiscSize/2-0]->setIcon(QIcon(QPixmap(":/Image/WhiteChess.png"))); Discs[DiscSize/2-0][DiscSize/2-0]->type = White;
    Discs[DiscSize/2-1][DiscSize/2-0]->setIcon(QIcon(QPixmap(":/Image/BlackChess.png"))); Discs[DiscSize/2-1][DiscSize/2-0]->type = Black;
    Discs[DiscSize/2-0][DiscSize/2-1]->setIcon(QIcon(QPixmap(":/Image/BlackChess.png"))); Discs[DiscSize/2-0][DiscSize/2-1]->type = Black;


    NumberOfBlack = 2;
    NumberOfWhite = 2;
    Player = Black;
    Enemy  = White;

    totalMove = 0;
    ui->BlackNumber->display(NumberOfBlack);
    ui->WhiteNumber->display(NumberOfWhite);
    //game = othello("127.0.0.1", 6068);
    //Start();

    if(NET == (PlayerType)(ui->BlackAI->currentIndex())){
        //game = othello("127.0.0.1", 6068);
        CON();
    }
    else if ( NET == (PlayerType)(ui->WhiteAI->currentIndex())){
        //game = othello("127.0.0.1", 6068);
        CON();
    } else {
        AI();
    }

}

void MainWindow::CON(){

    cout << "CONect starting..." << endl;
    auto codendata = game.wait_for_turn();
    auto code = codendata.first;
    auto data = codendata.second;
    cout << code << endl;
    // get my turn
    if (code == "turn") {
        AI();
    }
    else if (code == "update") {
        string move = data["move"];

        cout << "This is enemy move: " << move << endl;
        cout << data["move"][0] << data["move"][1] << endl;

        getDropDisc(data["move"][0] - 49, data["move"][1] - 65);
    }
    else if (code == "end") {
        std::cout << data["score"] << std::endl;
        std::cout << data["status"] << std::endl;
    }
    else {
        exit(-1);
    }

}
void MainWindow::getDropDisc(int row, int column)
{
    if(Discs[row][column]->type != Empty){
        return;
    }

    int MaxEat = 0;

    for(int deltaY = -1; deltaY <= 1; deltaY++){
        for(int deltaX = -1; deltaX <= 1; deltaX++){

            if((deltaX == 0 && deltaY == 0) || row+deltaY == -1 || row+deltaY == DiscSize || column+deltaX == -1 || column+deltaX == DiscSize){
                continue;
            }

            int Eat = NumberOfDiscCanEat(row, column, deltaX, deltaY);

            if(Eat == 0){
                continue;
            }

            EatDiscs(row, column, deltaX, deltaY);

            if(Player == Black){
                NumberOfBlack += Eat;
                NumberOfWhite -= Eat;
            }else{
                NumberOfWhite += Eat;
                NumberOfBlack -= Eat;
            }

            if(MaxEat < Eat){
                MaxEat = Eat;
            }
        }
    }

    if (MaxEat == 0){
        return;
    }

    DropThisDisc(row, column);

}


int MainWindow::NumberOfDiscCanEat(int row, int column, int deltaX, int deltaY)
{
    int deltaRow = row + deltaY,
        deltaColumn = column + deltaX,
        eat = 0;

    while (deltaColumn != -1 && deltaRow != -1 && deltaColumn != DiscSize && deltaRow != DiscSize) {
        if(Discs[deltaRow][deltaColumn]->type != Enemy){
            break;
        }

        deltaRow += deltaY;
        deltaColumn += deltaX;
        eat++;
    }

    if(deltaColumn == -1 || deltaRow == -1 || deltaColumn == DiscSize || deltaRow == DiscSize){
        return 0;
    }

    if(Discs[deltaRow][deltaColumn]->type != Player || eat == 0){
        return 0;
    }

    return eat;
}

int MainWindow::NumberOfDiscCanEatTotal(int row, int column)
{
    int TotalEat = 0;
    for(int deltaY = -1; deltaY <= 1; deltaY++){
        for(int deltaX = -1; deltaX <= 1; deltaX++){

            if((deltaX == 0 && deltaY == 0) || row+deltaY == -1 || row+deltaY == DiscSize || column+deltaX == -1 || column+deltaX == DiscSize){
                continue;
            }

            TotalEat += NumberOfDiscCanEat(row, column, deltaX, deltaY);
        }
    }

    return TotalEat;
}

void MainWindow::DropThisDisc(int row, int column)
{
    cout << "calling drop this Disc" << endl;
    totalMove++;

    if(column == -1) {
        state.skip();

    }
    else {
        cout << "move: " << row << " " << column << endl;
        state.DoMove(row*DiscSize+column);

        if(Player == Black){
            Discs[row][column]->setIcon(QIcon(QPixmap(":/Image/BlackChess.png")));
            NumberOfBlack++;
        }else{
            Discs[row][column]->setIcon(QIcon(QPixmap(":/Image/WhiteChess.png")));
            NumberOfWhite++;
        }

        Discs[row][column]->type = Player;

    }
//    state.ShowBoard();
    //cout << " " << endl;

    ui->BlackNumber->display(NumberOfBlack);
    ui->WhiteNumber->display(NumberOfWhite);

    PlayerType pt, ept;
    if(Player == Black) {
        pt = (PlayerType)(ui->BlackAI->currentIndex());
        ept = (PlayerType)(ui->WhiteAI->currentIndex());
    } else {
        pt = (PlayerType)(ui->WhiteAI->currentIndex());
        ept = (PlayerType)(ui->BlackAI->currentIndex());
    }

    qSwap(Player, Enemy);


    if(state.is_end()) {
        delay(5000);
        Restart();
        return;
    }


    set<int> moves;
    state.GetAllMoves(moves);
    //std::cout << "nex valid moves: " << moves.size() << std::endl;


    if(moves.size() == 0) {
        DropThisDisc(-1, -1);
    }
    else if((pt == COMP && ept == NET) || (pt == NET && ept == COMP) ) {
        delay(200);
        CON();
    }else{
        delay(200);
        cout << "waitting for connecting enemy" << endl;
        AI();
    }

}

void MainWindow::EatDiscs(int row, int column, int deltaX, int deltaY)
{
    int deltaRow = row + deltaY,
        deltaColumn = column + deltaX;

    if(deltaRow != -1 && deltaColumn != -1 && deltaRow != DiscSize && deltaColumn != DiscSize){
        /* Start Eating */
        int EatRow = row + deltaY,
            EatColumn = column + deltaX;

        while (EatRow != -1 && EatColumn != -1 && EatRow != DiscSize && EatColumn != DiscSize && Discs[EatRow][EatColumn]->type != Player) {

            if(Player == Black){
                Discs[EatRow][EatColumn]->setIcon(QIcon(QPixmap(":/Image/BlackChess.png")));
                Discs[EatRow][EatColumn]->type = Black;
            }else{
                Discs[EatRow][EatColumn]->setIcon(QIcon(QPixmap(":/Image/WhiteChess.png")));
                Discs[EatRow][EatColumn]->type = White;
            }

            EatRow += deltaY;
            EatColumn += deltaX;
        }
    }
}

void MainWindow::updateTime()
{
    // *TimeRecord = TimeRecord->addSecs(1);
    // ui->Timer->display(TimeRecord->toString("mm:ss"));
}
void MainWindow::delay(int msec)
{
    QTime dieTime= QTime::currentTime().addMSecs(msec);

    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
void MainWindow::AI()
{

    // TimeRecord->setHMS(0,0,0);
    // ui->Timer->display(TimeRecord->toString("mm:ss"));
    cout << "calling ai" << endl;
    int MaxRow = 0, MaxColumn = 0;

    int m = -1;
    PlayerType pt, ept;
    if(Player == Black) {
        pt = (PlayerType)(ui->BlackAI->currentIndex());
        ept = (PlayerType)(ui->WhiteAI->currentIndex());
    } else {
        pt = (PlayerType)(ui->WhiteAI->currentIndex());
        ept = (PlayerType)(ui->BlackAI->currentIndex());
    }

    clock_t start_time;
    start_time = clock();
    int time_limit = 3;


    if(pt == COMP) {
        m = ai::step(state, start_time, time_limit);
    }else{
        return;
    }

    cout << "got move" << endl;
    TimeRecord->setHMS(0, (clock()-start_time)/(double)CLOCKS_PER_SEC/60, (clock()-start_time)/(double)CLOCKS_PER_SEC);
    ui->Timer->display(TimeRecord->toString("mm:ss"));

    MaxRow = m / DiscSize;
    MaxColumn = m % DiscSize;


    // send my move
    // A == 65 [행][렬] [row][column] ex)1H 0==1 7==H



    cout << "m row col: " << m << " " << MaxRow << " " << MaxColumn << endl;

    if (MaxColumn != -1) {
        for(int deltaY = -1; deltaY <= 1; deltaY++){
            for(int deltaX = -1; deltaX <= 1; deltaX++){

                if((deltaX == 0 && deltaY == 0) || MaxRow+deltaY == -1 || MaxRow+deltaY == DiscSize || MaxColumn+deltaX == -1 || MaxColumn+deltaX == DiscSize){
                    continue;
                }

                int Eat = NumberOfDiscCanEat(MaxRow, MaxColumn, deltaX, deltaY);

                if(Eat == 0){
                    continue;
                }

                EatDiscs(MaxRow, MaxColumn, deltaX, deltaY);

                if(Player == Black){
                    NumberOfBlack += Eat;
                    NumberOfWhite -= Eat;
                }else{
                    NumberOfWhite += Eat;
                    NumberOfBlack -= Eat;
                }
            }
        }
    }
    if(pt == NET || ept == NET){
        char num[11] = "0123456789";
        string ai_step = "";
        ai_step += num[MaxRow + 1];
        ai_step += (char)(MaxColumn + 65);
        cout << "this is AI move <<" << ai_step << endl;
        game.move(ai_step);

    }
    DropThisDisc(MaxRow, MaxColumn);
    if(pt == NET || ept == NET)
        CON();


}

void MainWindow::Refresh()
{
    PlayerType pt, ept;
    if(Player == Black) {
        pt = (PlayerType)(ui->BlackAI->currentIndex());
        ept = (PlayerType)(ui->WhiteAI->currentIndex());
    } else {
        pt = (PlayerType)(ui->WhiteAI->currentIndex());
        ept = (PlayerType)(ui->BlackAI->currentIndex());
    }
    NumberOfBlack = NumberOfWhite = 0;
    for(int i = 0; i < DiscSize; i++)
    {
        for(int j = 0; j < DiscSize; j++)
        {
            Discs[i][j]->setIcon(QIcon());
            if(Discs[i][j]->type == White){
                NumberOfWhite++;
                Discs[i][j]->setIcon(QIcon(QPixmap(":/Image/WhiteChess.png")));
            }else if(Discs[i][j]->type == Black){
                NumberOfBlack++;
                Discs[i][j]->setIcon(QIcon(QPixmap(":/Image/BlackChess.png")));
            }else{
                Discs[i][j]->setIcon(QIcon());
            }
        }
    }
    ui->BlackNumber->display(NumberOfBlack);
    ui->WhiteNumber->display(NumberOfWhite);
    if(pt == NET || ept == NET)
        CON();
}
