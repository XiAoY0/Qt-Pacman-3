//ghost函数实现
#include "game.h"
#include "chasing_strategy.cpp"
#include <QRandomGenerator>
#define W (GameObject::Width)

Ghost::Ghost(int clr) : GameObject(
    GameObject::Ghost, QPixmap())
{
    color = (Color)clr;
    anim_index = 0;
    is_released = false;
    dir = Dir(QRandomGenerator::global()->generate() % 4);
    status = Normal;

    //四种颜色ghost+ 不同状态动画
    switch (color) {
    case Red:
        anim[Right].push_back(QPixmap(":/game_objects/ghosts/redright1.png"));
        anim[Right].push_back(QPixmap(":/game_objects/ghosts/redright2.png"));
        anim[Up].push_back(QPixmap(":/game_objects/ghosts/redup1.png"));
        anim[Up].push_back(QPixmap(":/game_objects/ghosts/redup2.png"));
        anim[Left].push_back(QPixmap(":/game_objects/ghosts/redleft1.png"));
        anim[Left].push_back(QPixmap(":/game_objects/ghosts/redleft2.png"));
        anim[Down].push_back(QPixmap(":/game_objects/ghosts/reddown1.png"));
        anim[Down].push_back(QPixmap(":/game_objects/ghosts/reddown2.png"));
        break;
    case Yellow:
        anim[Right].push_back(QPixmap(":/game_objects/ghosts/yellowright1.png"));
        anim[Right].push_back(QPixmap(":/game_objects/ghosts/yellowright2.png"));
        anim[Up].push_back(QPixmap(":/game_objects/ghosts/yellowup1.png"));
        anim[Up].push_back(QPixmap(":/game_objects/ghosts/yellowup2.png"));
        anim[Left].push_back(QPixmap(":/game_objects/ghosts/yellowleft1.png"));
        anim[Left].push_back(QPixmap(":/game_objects/ghosts/yellowleft2.png"));
        anim[Down].push_back(QPixmap(":/game_objects/ghosts/yellowdown1.png"));
        anim[Down].push_back(QPixmap(":/game_objects/ghosts/yellowdown2.png"));
        break;
    case Green:
        anim[Right].push_back(QPixmap(":/game_objects/ghosts/greenright1.png"));
        anim[Right].push_back(QPixmap(":/game_objects/ghosts/greenright2.png"));
        anim[Up].push_back(QPixmap(":/game_objects/ghosts/greenup1.png"));
        anim[Up].push_back(QPixmap(":/game_objects/ghosts/greenup2.png"));
        anim[Left].push_back(QPixmap(":/game_objects/ghosts/greenleft1.png"));
        anim[Left].push_back(QPixmap(":/game_objects/ghosts/greenleft2.png"));
        anim[Down].push_back(QPixmap(":/game_objects/ghosts/greendown1.png"));
        anim[Down].push_back(QPixmap(":/game_objects/ghosts/greendown2.png"));
        break;
    case Pink:
        anim[Right].push_back(QPixmap(":/game_objects/ghosts/pinkright1.png"));
        anim[Right].push_back(QPixmap(":/game_objects/ghosts/pinkright2.png"));
        anim[Up].push_back(QPixmap(":/game_objects/ghosts/pinkup1.png"));
        anim[Up].push_back(QPixmap(":/game_objects/ghosts/pinkup2.png"));
        anim[Left].push_back(QPixmap(":/game_objects/ghosts/pinkleft1.png"));
        anim[Left].push_back(QPixmap(":/game_objects/ghosts/pinkleft2.png"));
        anim[Down].push_back(QPixmap(":/game_objects/ghosts/pinkdown1.png"));
        anim[Down].push_back(QPixmap(":/game_objects/ghosts/pinkdown2.png"));
        break;
    }

    panic_anim.push_back(QPixmap(":/game_objects/ghosts/bl1.png"));
    panic_anim.push_back(QPixmap(":/game_objects/ghosts/bl2.png"));
    running_anim.push_back(QPixmap(":/game_objects/ghosts/run.png"));
    setPixmap(anim[Right][0]);
}


Ghost::Color Ghost::get_color()
{
    return color;
}
//移动方法 ghost状态不同 采取不同移动策略
void Ghost::moveup()
{
    QVector<QPixmap> *ptr;
    switch (status) {
    case Normal:
        ptr = &anim[Up];
        break;
    case Panic:
        ptr = &panic_anim;
        break;
    case Running:
        ptr = &running_anim;
        break;
    }

    anim_index++;
    if (anim_index >= ptr->size()) {
        anim_index = 0;
    }
    setPixmap((*ptr)[anim_index]);
    setY(static_cast<int>(y()) - 1);
}

void Ghost::moveleft()
{
    QVector<QPixmap> *ptr;
    switch (status) {
    case Normal:
        ptr = &anim[Left];
        break;
    case Panic:
        ptr = &panic_anim;
        break;
    case Running:
        ptr = &running_anim;
        break;
    }

    anim_index++;
    if (anim_index >= ptr->size()) {
        anim_index = 0;
    }
    setPixmap((*ptr)[anim_index]);
    setX(static_cast<int>(x()) - 1);
}

void Ghost::movedown()
{
    QVector<QPixmap> *ptr;
    switch (status) {
    case Normal:
        ptr = &anim[Down];
        break;
    case Panic:
        ptr = &panic_anim;
        break;
    case Running:
        ptr = &running_anim;
        break;
    }

    //动画循环播放
    anim_index++;
    if (anim_index >= ptr->size()) {
        anim_index = 0;
    }
    //设置item显示图片
    setPixmap((*ptr)[anim_index]);
    setY(static_cast<int>(y()) + 1);
}

void Ghost::moveright()
{
    QVector<QPixmap> *ptr;
    switch (status) {
    case Normal:
        ptr = &anim[Right];
        break;
    case Panic:
        ptr = &panic_anim;
        break;
    case Running:
        ptr = &running_anim;
        break;
    }

    anim_index++;
    if (anim_index >= ptr->size()) {
        anim_index = 0;
    }
    setPixmap((*ptr)[anim_index]);
    //将幽灵在场景中的 y 坐标增加 1，表示向下移动
    setX(static_cast<int>(x()) + 1);
}

//判断是否重叠
bool Ghost::overlapable(int i, int j)
{
    //边界检查
    if (i < 0 || j < 0) {
        return false;
    }
    if (i >= game->map_height || j >= game->map_width) {
        return false;
    }
    //进一步判断墙和门 门在满足条件时能进
    switch (game->map[i][j]->get_type()) {
    case Wall:
        return false;
        //判断什么时候门能进
    case Gate:
        if (is_released == false && release_time == 0) {
            return true;
        } else if (status == Running) {
            return true;
        } else {
            return false;
        }
    default:
        return true;
    }
}

//随机设置方向 要不能是墙
void Ghost::setdir_randomly()
{
    QVector<Dir> oklist;
    if (overlapable(_y, _x + 1)) {
        oklist.push_back(Right);
    }
    if (overlapable(_y, _x - 1)) {
        oklist.push_back(Left);
    }
    if (overlapable(_y + 1, _x)) {
        oklist.push_back(Down);
    }
    if (overlapable(_y - 1, _x)) {
        oklist.push_back(Up);
    }
    //随机挑选方向
    dir = oklist.at(QRandomGenerator::global()->generate() % oklist.size());
}

//离开笼子方法
void Ghost::go_out_cage()
{
    //计算幽灵与门之间距离
    int x_dist_to_gate = game->gate->_x - _x;
    //垂直距离
    int y_dist_to_gate = game->gate->_y - _y;
    //根据相对位置确定移动方向
    if (x_dist_to_gate > 0) {
        set_dir(GameObject::Right);
    } else if (x_dist_to_gate < 0) {
        set_dir(GameObject::Left);
    } else {
        if (y_dist_to_gate > 0) {
            set_dir(GameObject::Down);
        } else {
            set_dir(GameObject::Up);
        }
    }
}


//追逐和躲避判断  还得改
void Ghost::chase_pacman()
{
    bool okdir[5] = {false, false, false, false, false};
    QVector<Dir> oklist;
    //首先判断幽灵的四个方向是不是都能走
    if (overlapable(_y, _x + 1)) {
        okdir[Right] = true;
        oklist.push_back(Right);
    }
    if (overlapable(_y, _x - 1)) {
        okdir[Left] = true;
        oklist.push_back(Left);
    }
    if (overlapable(_y + 1, _x)) {
        okdir[Down] = true;
        oklist.push_back(Down);
    }
    if (overlapable(_y - 1, _x)) {
        okdir[Up] = true;
        oklist.push_back(Up);
    }

    //确定反方向 避免进入死循环 （没解决）
    Dir backward_dir;
    switch (dir) {
    case Up:
        backward_dir = Down;
        break;
    case Down:
        backward_dir = Up;
        break;
    case Left:
        backward_dir = Right;
        break;
    case Right:
        backward_dir = Left;
        break;
    case Stop:
        backward_dir = Stop;
        break;
    }


    if (oklist.size() > 2) {
        QPair<int, int> vector = chase_strategy(this);
        int dist_x = vector.first;
        int dist_y = vector.second;
        //根据pacman的位置确定ghost的移动方向
        if (dist_y > 0) {
            if (okdir[Down] && dir != Up) {
                dir = Down;
            } else {
                if (dist_x >= 0) {
                    if (okdir[Right]) {
                        dir = Right;
                    } else if (okdir[Left]) {
                        dir = Left;
                    }
                } else {
                    if (okdir[Left]) {
                        dir = Left;
                    } else if (okdir[Right]) {
                        dir = Right;
                    }
                }
            }
        } else if (dist_y < 0) {
            if (okdir[Up] && dir != Down) {
                dir = Up;
            } else {
                if (dist_x >= 0) {
                    if (okdir[Right]) {
                        dir = Right;
                    } else if (okdir[Left]) {
                        dir = Left;
                    }
                } else {
                    if (okdir[Left]) {
                        dir = Left;
                    } else if (okdir[Right]) {
                        dir = Right;
                    }
                }
            }
        } else if (dist_x > 0) {
            if (okdir[Right] && dir != Left) {
                dir = Right;
            } else {
                if (okdir[Up]) {
                    dir = Up;
                } else if(okdir[Down]) {
                    dir = Down;
                }
            }
        } else if (dist_x < 0) {
            if (okdir[Left] && dir != Right) {
                dir = Left;
            } else {
                if (okdir[Up]) {
                    dir = Up;
                } else if(okdir[Down]) {
                    dir = Down;
                }
            }
        }
    } else if (oklist.size() == 2) {
        if (okdir[dir] == false) {
            // ghost 在角落
            if (oklist[0] == backward_dir) {
                dir = oklist[1];
            } else {
                dir = oklist[0];
            }
        }
    }
}

//跟上面类似 但是幽灵以躲避pacman为目的
void Ghost::dodge_pacman()
{

    //先检查各个方向是否能走通
    bool okdir[5] = {false, false, false, false, false};
    QVector<Dir> oklist;
    if (overlapable(_y, _x + 1)) {
        okdir[Right] = true;
        oklist.push_back(Right);
    }
    if (overlapable(_y, _x - 1)) {
        okdir[Left] = true;
        oklist.push_back(Left);
    }
    if (overlapable(_y + 1, _x)) {
        okdir[Down] = true;
        oklist.push_back(Down);
    }
    if (overlapable(_y - 1, _x)) {
        okdir[Up] = true;
        oklist.push_back(Up);
    }

    //反方向
    Dir backward_dir;
    switch (dir) {
    case Up:
        backward_dir = Down;
        break;
    case Down:
        backward_dir = Up;
        break;
    case Left:
        backward_dir = Right;
        break;
    case Right:
        backward_dir = Left;
        break;
    case Stop:
        backward_dir = Stop;
        break;
    }

    //确定新方向 选择远离pacman的方向移动
    if (oklist.size() > 2) {
        int dist_x = game->pacman->get_x() - _x;
        int dist_y = game->pacman->get_y() - _y;

    //     if (dist_y < 0) {
    //         if (okdir[Down] && dir != Up) {
    //             dir = Down;
    //         } else {
    //             if (dist_x >= 0) {
    //                 if (okdir[Left]) {
    //                     dir = Left;
    //                 } else if (okdir[Right]) {
    //                     dir = Right;
    //                 }
    //             } else {
    //                 if (okdir[Right]) {
    //                     dir = Right;
    //                 } else if (okdir[Left]) {
    //                     dir = Left;
    //                 }
    //             }
    //         }
    //     } else if (dist_y > 0) {
    //         if (okdir[Up] && dir != Down) {
    //             dir = Up;
    //         } else {
    //             if (dist_x >= 0) {
    //                 if (okdir[Left]) {
    //                     dir = Left;
    //                 } else if (okdir[Right]) {
    //                     dir = Right;
    //                 }
    //             } else {
    //                 if (okdir[Right]) {
    //                     dir = Right;
    //                 } else if (okdir[Left]) {
    //                     dir = Left;
    //                 }
    //             }
    //         }
    //     } else if (dist_x < 0) {
    //         if(okdir[Right] && dir != Left) {
    //             dir = Right;
    //         } else {
    //             if (okdir[Up]) {
    //                 dir = Up;
    //             } else if(okdir[Down]) {
    //                 dir = Down;
    //             }
    //         }
    //     } else if (dist_x > 0) {
    //         if(okdir[Left] && dir != Right) {
    //             dir = Left;
    //         } else {
    //             if (okdir[Up]) {
    //                 dir = Up;
    //             } else if(okdir[Down]) {
    //                 dir = Down;
    //             }
    //         }
    //     }
    // } else if (oklist.size() == 2) {
    //     if (okdir[dir] == false) {
    //         // 如果在拐角处，避免选择反方向
    //         if (oklist[0] == backward_dir) {
    //             dir = oklist[1];
    //         } else {
    //             dir = oklist[0];
    //         }
    //     }
    // }
        QVector<Dir> preferred_dirs;

        // 根据pacman的位置添加优先选择的方向
        if (dist_y < 0 && okdir[Down] && dir != Up) {
            preferred_dirs.push_back(Down);
        } else if (dist_y > 0 && okdir[Up] && dir != Down) {
            preferred_dirs.push_back(Up);
        }
        if (dist_x < 0 && okdir[Right] && dir != Left) {
            preferred_dirs.push_back(Right);
        } else if (dist_x > 0 && okdir[Left] && dir != Right) {
            preferred_dirs.push_back(Left);
        }

        // 如果优先方向不为空，从优先方向中随机选择一个
        if (!preferred_dirs.isEmpty()) {
            dir = preferred_dirs.at(QRandomGenerator::global()->generate() % preferred_dirs.size());
        } else {
            // 否则从所有可行方向中随机选择一个
            dir = oklist.at(QRandomGenerator::global()->generate() % oklist.size());
        }
    } else if (oklist.size() == 2) {
        if (okdir[dir] == false) {
            // 如果在拐角处，避免选择反方向
            if (oklist[0] == backward_dir) {
                dir = oklist[1];
            } else {
                dir = oklist[0];
            }
        }
    } else if (oklist.size() == 1) {
        dir = oklist[0];
    } else {
        // 如果没有可行方向，停下
        dir = Stop;
    }
}

void Ghost::go_to_cage()
{
    bool okdir[5] = {false, false, false, false, false};
    QVector<Dir> oklist;
    if (overlapable(_y, _x + 1)) {
        okdir[Right] = true;
        oklist.push_back(Right);
    }
    if (overlapable(_y, _x - 1)) {
        okdir[Left] = true;
        oklist.push_back(Left);
    }
    if (overlapable(_y + 1, _x)) {
        okdir[Down] = true;
        oklist.push_back(Down);
    }
    if (overlapable(_y - 1, _x)) {
        okdir[Up] = true;
        oklist.push_back(Up);
    }

    Dir backward_dir;
    switch (dir) {
    case Up:
        backward_dir = Down;
        break;
    case Down:
        backward_dir = Up;
        break;
    case Left:
        backward_dir = Right;
        break;
    case Right:
        backward_dir = Left;
        break;
    case Stop:
        backward_dir = Stop;
        break;
    }

    // Change direction only when ghost is
    // not at an intersection in order to
    // avoid oscillation.
    if (oklist.size() > 2) {
        int dist_x = game->gate->get_x() - _x;
        int dist_y = (game->gate->get_y() + 1) - _y;
        if (dist_y > 0) {
            if (okdir[Down] && dir != Up) {
                dir = Down;
            } else {
                if (dist_x >= 0) {
                    if (okdir[Right]) {
                        dir = Right;
                    } else if (okdir[Left]) {
                        dir = Left;
                    }
                } else {
                    if (okdir[Left]) {
                        dir = Left;
                    } else if (okdir[Right]) {
                        dir = Right;
                    }
                }
            }
        } else if (dist_y < 0) {
            if (okdir[Up] && dir != Down) {
                dir = Up;
            } else {
                if (dist_x >= 0) {
                    if (okdir[Right]) {
                        dir = Right;
                    } else if (okdir[Left]) {
                        dir = Left;
                    }
                } else {
                    if (okdir[Left]) {
                        dir = Left;
                    } else if (okdir[Right]) {
                        dir = Right;
                    }
                }
            }
        } else if (dist_x > 0) {
            if(okdir[Right] && dir != Left) {
                dir = Right;
            } else {
                if (okdir[Up]) {
                    dir = Up;
                } else if (okdir[Down]) {
                    dir = Down;
                }
            }
        } else if (dist_x < 0) {
            if(okdir[Left] && dir != Right) {
                dir = Left;
            } else {
                if (okdir[Up]) {
                    dir = Up;
                } else if (okdir[Down]) {
                    dir = Down;
                }
            }
        }
    } else if (oklist.size() == 2) {
        if (okdir[dir] == false) {
            // ghost is on a corner
            if (oklist[0] == backward_dir) {
                dir = oklist[1];
            } else {
                dir = oklist[0];
            }
        }
    }
}


void Ghost::move()
{
    if (status != Running && release_time > 0) {
        release_time--;
    } else if (status == Panic) {
        panic_time--;
        if (panic_time <= 0) {
            status = Normal;
            game->ghost_timer[color]->setInterval(NORMAL_INTERVAL);
        }
    }

    int gh_x = static_cast<int>(x());
    int gh_y = static_cast<int>(y());
    int __x = (gh_x - game->geo_x) / W;           // block x coordinate in map
    int __y = (gh_y - game->geo_y) / W;           // block y coordinate in map
    int x_remainder = (gh_x - game->geo_x) % W;   // remainder x pixel to fit a block
    int y_remainder = (gh_y - game->geo_y) % W;   // remainder y pixel to fit a block

    /* When ghost completely fits a block,
     * decide whether to change direction. */
    if (x_remainder == 0 && y_remainder == 0) {
        // update ghost's coordinate in map
        _x = __x;
        _y = __y;
        if (is_released) {
            /* Chase pacman. */
            switch (status) {
            case Normal:
                chase_pacman();
                break;
            case Panic:
                dodge_pacman();
                break;
            default:
                break;
            }
        } else {
            if (release_time == 0) {
                /* Time to go out the cage. */
                go_out_cage();
                if (game->map[_y][_x]->get_type() == Gate) {
                    // If arriving at gate, turn into release.
                    is_released = true;
                }
            } else {
                if (status == Running) {
                    if (_x == game->gate->get_x() && _y == game->gate->get_y() + 1) {
                        status = Normal;
                        game->ghost_timer[color]->setInterval(NORMAL_INTERVAL);
                    } else {
                        /* Go back to cage. */
                        go_to_cage();
                    }
                } else {
                    /* Keep staying in the cage. */
                    setdir_randomly();
                }
            }
        }
    }

    switch (dir) {
    case Stop:
        break;
    case Up:
        moveup();
        break;
    case Down:
        movedown();
        break;
    case Left:
        moveleft();
        break;
    case Right:
        moveright();
        break;
    }

    /* Test if hitting pacman */
    if (collidesWithItem(game->pacman, Qt::IntersectsItemShape)){
        if (status == Normal) {
            game->stat = Game::Lose;
        } else if (status == Panic) {
            setPixmap(QPixmap(":/rsc2/run.png"));
            game->score += score;
            status = Running;
            release_time = 200;
            is_released = false;
            game->ghost_timer[color]->setInterval(RUNNING_INTERVAL);
        }
    }
}
