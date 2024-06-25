#include "game.h"
#include <QString>
#include <QFile>
#include <QRandomGenerator>
#define W (GameObject::Width)

bool isStop=false;
int GHOST_RELEASE_TIME[] = {0, 200, 400, 600}; // 幽灵出笼时间间隔
//构造函数
Game::Game(int x, int y, int map_w, int map_h, QString map_src)
    : QGraphicsScene(x, y, W * map_w, W * map_h), map_src(map_src)
{
    geo_x = x;
    geo_y = y;
    stat = Playing;

    // 初始化地图
    map_size = map_w * map_h;
    map_width = map_w;
    map_height = map_h;
    map = new GameObject**[map_height];
    for (int i = 0; i < map_height; i++) {
        map[i] = new GameObject*[map_width];
        for (int j = 0; j < map_width; j++)
            map[i][j] = nullptr;
    }

    // 调用initMap初始化地图
    initMap();
}
//初始化游戏 用来重置游戏
void Game::init()
{
    stop();
    qDebug()<<"stoped";
    clearMap();
    qDebug()<<"map cleared";
    initMap();
    qDebug()<<"Map inited";
    start();
    qDebug()<<"game start";
}

void Game::initMap()
{
    // 初始化地图元素的代码，和之前的构造函数中相同
    ball_num = eat_num = score = 0;
    int ghost_i = 0;
    QPixmap wallpix(":/game_objects/map_objects/wall.png");
    QPixmap ballpix(":/game_objects/map_objects/dot.png");
    QPixmap powerballpix(":/game_objects/map_objects/power_ball.png");
    QPixmap gatepix(":/game_objects/map_objects/gate.png");
    QPixmap blankpix;
    QFile mapfile(map_src);
    mapfile.open(QIODevice::ReadOnly | QIODevice::Text);

    pacman = new Pacman();

    for (int i = 0; i < map_height; i++) {
        QByteArray line = mapfile.readLine();
        for (int j = 0; j < map_width; j++) {
            int tmp_x = geo_x + (j * W);
            int tmp_y = geo_y + (i * W);
            switch (line[j]) {
            case '1':
                map[i][j] = new GameObject(GameObject::Wall, wallpix);
                map[i][j]->setPos(tmp_x, tmp_y);
                //调用QGraphicsItem的方法
                addItem(map[i][j]);
                break;
            case '0':
                map[i][j] = new GameObject(GameObject::Ball, ballpix);
                map[i][j]->set_score(BALL_SCORE);
                map[i][j]->setPos(tmp_x, tmp_y);
                addItem(map[i][j]);
                ball_num++;
                break;
            case '4':
                map[i][j] = new GameObject(GameObject::PowerBall, powerballpix);
                map[i][j]->set_score(POWERBALL_SCORE);
                map[i][j]->setPos(tmp_x, tmp_y);
                addItem(map[i][j]);
                powerball.push_back(map[i][j]);
                ball_num++;
                break;
            case '3':
                map[i][j] = new GameObject(GameObject::Blank, blankpix);
                break;
            case '2':
                gate = new GameObject(GameObject::Gate, gatepix);
                gate->_x = j;
                gate->_y = i;
                gate->setPos(tmp_x, tmp_y);
                addItem(gate);
                map[i][j] = gate;
                break;
            case 'p':
                pacman = new Pacman();
                pacman->game = this;
                pacman->setZValue(2);
                pacman->setPos(tmp_x, tmp_y);
                addItem(pacman);
                map[i][j] = pacman;
                break;
            case 'g':
                map[i][j] = new GameObject(GameObject::Blank, blankpix);
                ghost[ghost_i] = new Ghost(ghost_i);
                ghost[ghost_i]->game = this;
                ghost[ghost_i]->setZValue(2);
                ghost[ghost_i]->release_time = GHOST_RELEASE_TIME[ghost_i];
                ghost[ghost_i]->_x = j;
                ghost[ghost_i]->_y = i;
                ghost[ghost_i]->set_score(GHOST_SCORE);
                ghost[ghost_i]->setPos(tmp_x, tmp_y);
                addItem(ghost[ghost_i]);
                ghost_i++;
                break;
            }
            if (map[i][j]) {
                map[i][j]->_x = j;
                map[i][j]->_y = i;
            }
        }
    }

    ghost[Ghost::Red]->chase_strategy = &strategy1;
    ghost[Ghost::Pink]->chase_strategy = &strategy2;
    ghost[Ghost::Green]->chase_strategy = &strategy3;
    ghost[Ghost::Yellow]->chase_strategy = &strategy4;
}
// void Game::clearMap()
// {
//     // 清除所有地图元素
//     for (int i = 0; i < map_height; i++) {
//         for (int j = 0; j < map_width; j++) {
//             if (map[i][j] != nullptr) {
//                 if (map[i][j]->scene() == this){
//                     removeItem(map[i][j]);
//                 }
//             }
//             delete map[i][j];
//             map[i][j] = nullptr;
//         }
//     }

//     // 清除大力丸和幽灵
//     powerball.clear();
//     for (int i = 0; i < Ghost::GhostNum; i++) {
//         if (ghost[i] != nullptr) {
//             if (ghost[i]->scene() == this){
//                 removeItem(ghost[i]);
//             }
//         }
//         delete ghost[i];
//         ghost[i] = nullptr;
//     }
//     // 清除Pacman
//     if (pacman != nullptr) {
//         if (pacman->scene() == this){
//              removeItem(pacman);
//         }
//     }
//     delete pacman;
//     pacman = nullptr;
// }
void Game::clearMap() {
    // 清除所有地图元素
    for (int i = 0; i < map_height; i++) {
        for (int j = 0; j < map_width; j++) {
            delete map[i][j]; // 删除对象以释放内存
            map[i][j] = nullptr;
        }
    }

    // 清除大力丸
    for (auto power : powerball) {
        delete power;
    }
    powerball.clear();

    // 清除幽灵
    for (int i = 0; i < Ghost::GhostNum; i++) {
        delete ghost[i];
        ghost[i] = nullptr;
    }

    // 清除Pacman
    delete pacman;
    pacman = nullptr;

    // 清空场景
    clear();
}
void Game::start()
{
    // 创建大力丸闪烁定时器
    powerball_flash_timer = new QTimer(this);

    // 连接定时器超时信号到能量豆闪烁槽函数
    connect(powerball_flash_timer, SIGNAL(timeout()), this , SLOT(powerball_flash()));
    powerball_flash_timer->start(FLASH_INTERVAL); // 启动定时器

    // 创建Pacman定时器
    pacman_timer = new QTimer(this);
    connect(pacman_timer, SIGNAL(timeout()), this , SLOT(pacman_handler()));
    pacman_timer->start(INTERVAL); // 启动定时器

    // 遍历所有幽灵
    for (int i = 0; i < Ghost::GhostNum; i++) {
        ghost_timer[i] = new QTimer(this);
        // 连接同时传参 ghost-id
        connect(ghost_timer[i], &QTimer::timeout, [=](){ghost_handler(i);} );
        ghost_timer[i]->start(NORMAL_INTERVAL);
    }
}

void Game::stop()
{
    pacman_timer->stop(); // 停止Pacman定时器
    powerball_flash_timer->stop(); // 停止能量豆闪烁定时器
    for (int i = 0; i < Ghost::GhostNum; i++) { // 遍历所有幽灵
        ghost_timer[i]->stop(); // 停止幽灵定时器
    }
}
//重新启动
void Game::restart()
{
    pacman_timer->start();//启动pacman计时器
    powerball_flash_timer->start();
    for (int i=0;i<Ghost::GhostNum;i++){
        ghost_timer[i]->start();
    }
}
//所有大力丸已经被吃掉 隐藏相关资源
//没有则显示
void Game::powerball_flash()
{
    if (powerball.empty()) {
        powerball_flash_timer->stop();
        return;
    }

    if (flash_tick) {
        for (int i = 0; i < powerball.size(); i++) {
            powerball.at(i)->hide();
        }
        flash_tick = 0;
    } else {
        for (int i = 0; i < powerball.size(); i++) { // 遍历所有能量豆
            powerball.at(i)->show(); // 显示能量豆
        }
        flash_tick = 1; // 设置闪烁标志
    }
}
//游戏暂停
void Game::pause_restart()
{
    if(!isStop)
    {
        stop();
        isStop=true;
    }
    else
    {
        start();
        isStop=false;
    }

}
void Game::pacman_handler()
{
    pacman->move();
    // 如果游戏状态为Win 停止游戏
    if (stat == Win) {
        stop();
    }
}

//多一个ghost id 指明哪个ghost
void Game::ghost_handler(int ghost_id)
{
    ghost[ghost_id]->move();
    // 如果游戏状态为Lose  停止游戏

    //TODO：弹窗或者按钮显示重新开始游戏
    if (stat == Lose) {
        stop();
    }
}

// 设置Pacman的下一个方向
void Game::pacman_next_direction(GameObject::Dir d)
{
    pacman->set_next_dir(d);
}

int Game::get_score()
{
    return score;
}

Game::~Game()
{
    for (int i = 0; i < map_height; i++) { // 遍历地图高度
        for (int j = 0; j < map_width; j++) { // 遍历地图宽度
            if (map[i][j] != nullptr) // 如果地图中有对象
                delete map[i][j]; // 删除对象
        }
        delete[] map[i]; // 删除行指针数组
    }
    delete[] map; // 删除地图指针数组

    // 删除Pacman定时器
    delete pacman_timer;
    delete powerball_flash_timer;

    // 删除幽灵定时器
    for (int i = 0; i < Ghost::GhostNum; i++) {
        delete ghost_timer[i];
    }
}
