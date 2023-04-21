#pragma once
#include <iostream>

const int BOARD_COL = 30;
const int BOARD_ROW = 15;
const int GRID_WIDTH = 2;

enum GameStatus { S_OVER, S_NORMAL, S_QUIT };

void initialize();

void shutdown();

class Gobang {
public:
    Gobang() : status(S_NORMAL), currLocal(std::make_pair(0, 0)), who(true) {}
    void start();

private:
    // 游戏面板
    void draw_board();
    // 用户输入控制
    void processInput();
    // 绘制特定字符
    void drawch(const int& row, const int& col, const char& ch);
    // 重新开始
    void restart();
    // 胜利判断
    bool isOver();
    // 返回游戏状态
    GameStatus getStatus();

private:
    // 游戏状态
    GameStatus status;
    // 储存落子下标
    int gridVal[BOARD_ROW][BOARD_COL] = {' '};
    // 当前光标位置
    std::pair<int, int> currLocal;
    // 用于用户交替判断
    bool who;
};
