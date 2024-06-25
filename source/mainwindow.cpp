#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMediaPlayer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("pac-man"));

    setWindowIcon(QIcon(":/game_objects/ghosts/redright1.png"));
    /* Initialize graphicsview and game scene */
    ui->graphicsView->setStyleSheet("QGraphicsView {border: none;}");
    ui->graphicsView->setBackgroundBrush(Qt::black);
    ui->graphicsView->setFocusPolicy(Qt::NoFocus);

    victoryPlayer = new QMediaPlayer(this);
    defeatPlayer = new QMediaPlayer(this);

    victoryPlayer->setSource(QUrl(":/sounds/game_objects/sounds/ready.mp3"));
    defeatPlayer->setSource(QUrl(":/sounds/game_objects/sounds/die.mp3"));

    int map_height = 20, map_width = 29;            // 20x29
    int x = 50, y = 50;                             // x y
    int w = (map_width * GameObject::Width);
    int h = (map_height * GameObject::Width);

    ui->graphicsView->setGeometry(x, y, w, h);
    game = new Game(x, y, map_width, map_height, ":/game_objects/map_objects/map.txt");
    ui->graphicsView->setScene(game);
    initLabels();
    game->start();
}


/* Initialize UI */
void MainWindow::initLabels()
{
    score_title = new QLabel(this);
    score_title->setText("score");
    score_title->setStyleSheet("QLabel {font-family: Fixedsys;color: white;font-size: 16px;}");
    score_title->setGeometry(50, 12, 60, 26);

    score = new QLabel(this);
    score->setIndent(-80);
    score->setText("0");
    score->setStyleSheet("QLabel {font-family: Fixedsys;color: white;font-size: 16px;}");
    score->setGeometry(110, 12, 150, 26);

    win_label = new QLabel(this);
    win_label->hide();
    win_label->setText("You win!");
    win_label->setStyleSheet("QLabel {font-family: Fixedsys;color: yellow;font-size: 16px;}");
    win_label->setGeometry(310, 12, 150, 26);

    lose_label = new QLabel(this);
    lose_label->hide();
    lose_label->setText("You lose!");
    lose_label->setStyleSheet("QLabel {font-family: Fixedsys;color: red;font-size: 16px;}");
    lose_label->setGeometry(310, 12, 150, 26);

    score_timer = new QTimer(this);
    score_timer->start(25);
    connect(score_timer, SIGNAL(timeout()), this , SLOT(update_score()));
}


/* Update score UI */
void MainWindow::update_score()
{
    score->setText(QString::number(game->get_score()));
    if (game->stat == Game::Win) {
        win_label->show();
        score_timer->stop();
        victoryPlayer->play();
        //QTimer::singleShot(3000, this, &MainWindow::reset_game); // 3秒后重置游戏

    } else if (game->stat == Game::Lose) {
        lose_label->show();
        score_timer->stop();
        //QTimer::singleShot(3000, this, &MainWindow::reset_game); // 3秒后重置游戏
        defeatPlayer->play();
    }
}

/*reset the game*/
void MainWindow::reset_game()
{
    game->init(); // 调用Game类的reset方法重置游戏
}
void MainWindow::keyPressEvent(QKeyEvent *e) {
    switch (e->key()) {
    case Qt::Key_W:
        game->pacman_next_direction(GameObject::Up);
        break;
    case Qt::Key_A:
        game->pacman_next_direction(GameObject::Left);
        break;
    case Qt::Key_S:
        game->pacman_next_direction(GameObject::Down);
        break;
    case Qt::Key_D:
        game->pacman_next_direction(GameObject::Right);
        break;
    //空格暂停游戏 空格重新开始游戏
    case Qt::Key_Space:
        game->pause_restart();
        break;
    case Qt::Key_R:
        //reset_game();
        game->~Game();
        //game->Game();
        break;

    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

