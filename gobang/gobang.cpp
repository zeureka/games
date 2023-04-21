#include "gobang.h"
#include <curses.h>
#include <cstring>
#include <ncurses.h>

void initialize() {
    initscr(); // 初始化 curses
    cbreak();  // 不需要回车，直接交互
    noecho();  // 按键不显示
    start_color(); // 启用颜色显示
    curs_set(1);   // 显示光标
}

void shutdown() {
    endwin(); // 清理 curses
}

void Gobang::start() {
    initialize();
    do {
        char str[] = "Welcom to Gobang Game";
        char start[] = "Press 's/S' to start the game.";
        char quit[] = "Press 'q/Q' to exit the game.";
        mvprintw(1, (COLS - strlen(str)) / 2, "%s", str);
        mvprintw(3, (COLS - strlen(quit)) / 2, "%s", start);
        mvprintw(5, (COLS - strlen(quit)) / 2, "%s", quit);
        char ch = getch();

        if ('q' == ch || 'Q' == ch) {
          break;
        }

        do {
            this->draw_board();
            this->processInput();
        } while(this->getStatus() != S_QUIT);

        clear();
        refresh();
    } while (true);

    shutdown();
}


void Gobang::drawch(const int& row, const int& col, const char& ch) {
    move(row, col);
    printw("%c", ch);
}


void Gobang::draw_board() {
    clear();
    // 创建前景色与背景色键值对
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);

    for (int i = 0; i <= BOARD_ROW; ++i) {
        for (int j = 0; j <= BOARD_COL; ++j) {
            // 绘制落子点（交点）
            drawch(i * 2, j * (GRID_WIDTH + 1), '+');

            // 绘制边框
            for (int k = 1; j != BOARD_COL && k <= 2; ++k) {
                drawch(i * 2, j * (GRID_WIDTH + 1) + k, '-');
            }

            // 绘制边框
            if (i != BOARD_ROW) {
                drawch(i * 2 + 1, j * (GRID_WIDTH + 1), '|');
            }

            // 绘制棋子
            if ('#' == this->gridVal[i][j] || '@' == this->gridVal[i][j]) {
                int color = 0;
                if ('@' == this->gridVal[i][j]) {
                    color = 1;
                } else {
                    color = 2;
                }
                attron(COLOR_PAIR(color)); // 选用不同的颜色键值对
                drawch(i * 2, j * (GRID_WIDTH + 1), this->gridVal[i][j]);
                attroff(COLOR_PAIR(color));
            }

        }
    }

    attron(COLOR_PAIR(3));
    char str[] = "W(Up), S(Down), A(Left), D(Right), R(Restart), Q(Quit), C(Click)";
    mvprintw(2 * (BOARD_ROW + 2), 2, "%s", str);
    attroff(COLOR_PAIR(3));

    // 判断游戏是否结束
    if (S_OVER == status) {
        attron(COLOR_PAIR(4));
        mvprintw(2 * (BOARD_ROW + 3), 10, "Game Over");
        attroff(COLOR_PAIR(4));
    }

    move(this->currLocal.first * 2, this->currLocal.second * 3);
}

void Gobang::processInput() {
input:
    char ch = getch();
    ch = toupper(ch);

    if (S_NORMAL == status) {
        if ('A' == ch) {
            this->currLocal.second -= 1;
        } else if ('D' == ch) {
            this->currLocal.second += 1;
        } else if ('W' == ch) {
            this->currLocal.first -= 1;
        } else if ('S' == ch) {
            this->currLocal.first += 1;
        } else if ('C' == ch){
            char curCh = this->gridVal[this->currLocal.first][this->currLocal.second];
            // 重复落子
            while ('#' == curCh || '@' == curCh) {
                goto input;
            }

            char tmp;
            if (this->who) {
                tmp = '#';
            } else {
                tmp = '@';
            }

            // 更新棋子数组
            this->gridVal[this->currLocal.first][this->currLocal.second] = tmp;
            // 转换下棋方
            this->who = !this->who;
        }

    }

    // 判断是否胜利
    if (isOver()) {
        this->status = S_OVER;
    }

    if ('Q' == ch) {
        this->status = S_QUIT;
    } else if ('R' == ch) {
        this->restart();
    }
}

void Gobang::restart() {
    for (auto& ans : this->gridVal) {
        for (auto& val : ans) {
            val = ' ';
        }
    }
    this->status = S_NORMAL;
}

bool Gobang::isOver() {
    bool flag = false;

    // 上 右上 右 右下
    int dir[4][2]{{-1, 0}, {-1, 1}, {0, 1}, {1, 1}};
    for (int i = 0; i < 4; ++i) {
        int count = -1;
        int row = this->currLocal.first;
        int col = this->currLocal.second;
        char currCh = gridVal[this->currLocal.first][this->currLocal.second];
        // 选取一个方向一直迭代判断是否跟当前位置的棋子想等
        while (gridVal[row][col] == currCh && ('#' == currCh || '@' == currCh)) {
            ++count;
            row += dir[i][0];
            col += dir[i][1];
        }

        row = this->currLocal.first;
        col = this->currLocal.second;
        // 上一个方向的反方向迭代
        while (gridVal[row][col] == currCh && ('#' == currCh || '@' == currCh)) {
            ++count;
            row -= dir[i][0];
            col -= dir[i][1];
        }

        // 如果有连续 5 个以上相同的棋子
        if (count >= 5) {
            flag = true;
            break;
        }
    }

    return flag;
}

GameStatus Gobang::getStatus() { return this->status; }
