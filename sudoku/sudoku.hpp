#include <cctype>
#include <curses.h>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ncurses.h>
#include <utility>

enum class Level : char { FIRST, SECOND, THIRD, FOURTH, FIFTH };
enum class Status : char { WIN, FAIL, NORMAL, QUIT };

class Sudoku {
public:
    Sudoku(Level level = Level::FIRST);
    ~Sudoku() = default;
    void start(); // 开始游戏

private:
    void processInput();                            // 按键处理
    void drawWindow();                              // 绘制棋盘
    void copyHoleToGame();                          // 将 hole 数组中的值 copy 到 game 数组中
    void zeroData();                                // 将三个数组的值全部置为0
    void newGame();                                 // 开始新的棋盘游戏
    bool isOver();                                  // 判断游戏是否结束
    void initialize();                              // 初始化屏幕
    void shutdown();                                // 清理 ncurses
    void setLevel(Level level);                     // 设置游戏等级
    void resetNum(const int& x, const int& y);      // 回溯，将 (x, y) 位置的值改为0
    void digHole();                                 // 设置 hole 数组的值
    bool fillFrom(const int& y, const int& val);    // 设置 grid 数组
    void initXOrd(int* xOrd);                       // 设置一行的随机序列，此随机序列是[1, 9]元素在数组中的下标
    bool setNum(const int& x, const int& y, const int& val);    // 设置一个位置的值

private:
    int grid[9][9]; // 完整的棋盘数组
    int hole[9][9]; // 去除某些 holeCnt 个数据之后的棋盘数组
    int game[9][9]; // 保存游戏实时数据的棋盘数组
    Level level;    // 游戏等级
    int holeCnt;    // 游戏等级对应的去除数据个数
    Status status;  // 游戏状态
    std::pair<int, int> curLocal;  // 当前光标位置
};

/***************************************************************************/

void Sudoku::initialize() {
    initscr();
    cbreak();
    noecho();
    start_color();
    curs_set(1);
}

void Sudoku::shutdown() {
    endwin();
}

void Sudoku::zeroData() {
    bzero(grid, sizeof(grid));
    bzero(hole, sizeof(hole));
    bzero(game, sizeof(game));
}

void Sudoku::copyHoleToGame() {
    for (int y = 0; y < 9; ++y) {
        for (int x = 0; x < 9; ++x) {
            game[y][x] = hole[y][x];
        }
    }
}

void Sudoku::resetNum(const int& x, const int& y) {
    grid[y][x] = 0;
}

bool Sudoku::isOver () {
    for (int y = 0; y < 9; ++y) {
        for (int x = 0; x < 9; ++x) {
            if (game[y][x] != grid[y][x]) {
                return false;
            }
        }
    }
    return true;
}

void Sudoku::initXOrd(int* xOrd) {
    for (int i = 0; i < 9; ++i) {
        xOrd[i] = i;
    }

    int k = 0;
    for (int i = 0; i < 9; ++i) {
        k = rand() % 9;
        std::swap(xOrd[k], xOrd[i]);
    }
}

void Sudoku::newGame() {
    status = Status::NORMAL;
    zeroData();
    srand((unsigned)time(NULL));
    while(!fillFrom(0, 1));
    digHole();
}

Sudoku::Sudoku(Level level) {
    zeroData();
    status = Status::NORMAL;
    setLevel(level);
    curLocal.first = 0;
    curLocal.second = 0;
}

void Sudoku::setLevel(Level level) {
    this->level = level;
    switch(level) {
    case Level::FIRST:
        holeCnt = 45;
        break;
    case Level::SECOND:
        holeCnt = 50;
        break;
    case Level::THIRD:
        holeCnt = 55;
        break;
    case Level::FOURTH:
        holeCnt = 60;
        break;
    case Level::FIFTH:
        holeCnt = 65;
        break;
    default:
        break;
    }
}

bool Sudoku::setNum(const int& x, const int& y, const int& val) {
    if (grid[y][x] != 0) { // 非空
        return false;
    }

    int x0 = 0, y0 = 0;

    for (x0 = 0; x0 < 9; ++x0) {
        if (val == grid[y][x0]) {  // 行冲突
            return false;
        }
    }

    for (y0 = 0; y0 < 9; ++y0) {
        if (val == grid[y0][x]) {  // 列冲突
            return false;
        }
     }

    x0 = x / 3;  // 水平方向上的第几个九宫格
    y0 = y / 3;  // 垂直方向上的第几个九宫格

    for (int cX = x0 * 3; cX < x0 * 3 + 3; ++cX) {
        for (int cY = y0 * 3; cY < y0 * 3 + 3; ++cY) {
            if (val == grid[cY][cX]) {  // 格冲突
                return false;
            }
        }
    }

    grid[y][x] = val;
    return true;
}

bool Sudoku::fillFrom(const int& y, const int& val) {
    int xOrd[9];
    initXOrd(xOrd);  // 生成当前行的无序序列

    for (int i = 0; i < 9; ++i) {
        int x = xOrd[i];

        if (setNum(x, y, val)) {
            if (8 == y) {
                // 当前填9则结束, 否则从第一行填下一个数
                if (9 == val || fillFrom(0, val + 1)) {
                    return true;
                }
            } else {
                if(fillFrom(y + 1, val)) {  // 绘制棋盘格中每一行的 val 值
                    return true;
                }
            }

            resetNum(x, y);
        }
    }

    return false;
}

void Sudoku::digHole() {
    for (int i = 0; i < 81; ++i) {
        hole[i / 9][i % 9] = grid[i / 9][i % 9];
    }

    int k = 0;
    for (int i = 0; i < holeCnt; ++i) {
flag:
        k = rand() % 81;
        if (0 == hole[k / 9][k % 9]) {
            goto flag;
        }

        hole[k / 9][k % 9] = 0;
    }

    copyHoleToGame();
}

void Sudoku::drawWindow() {
    clear();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    for (int y = 0; y <= 9; ++y) {
        for (int x = 0; x <= 9; ++x) {
            // 绘制竖线
            if (3 == x || 6 == x || 0 == x || 9 == x) { attron(COLOR_PAIR(1)); }
            if (y != 9) { mvprintw(y * 2 + 1, x * 4, "%c", '|'); }
            if (3 == x || 6 == x || 0 == x || 9 == x) { attroff(COLOR_PAIR(1)); }

            // 绘制横线
            int k = (x == 0) ? 1 : 0;
            if (3 == y || 6 == y || 0 == y || 9 == y) { attron(COLOR_PAIR(1)); }
            for (; x != 9 && k <= 3; ++k) { mvprintw(y * 2, x * 4 + k, "%c", '-'); }
            if (3 == y || 6 == y || 0 == y || 9 == y) { attroff(COLOR_PAIR(1)); }

            if (y != 9 && x != 9 && game[y][x] != 0) {
                if (0 == hole[y][x]) { attron(COLOR_PAIR(2)); }
                mvprintw(y * 2 + 1, x * 4 + 2, "%d", game[y][x]);
                if (0 == hole[y][x]) { attroff(COLOR_PAIR(2)); }
            }
        }
    }
    
    if (status == Status::WIN) {
        attron(COLOR_PAIR(3));
        mvprintw(17, 40, "%s", "YOU WON!!!!!");
        attroff(COLOR_PAIR(3));
    } else if (status == Status::FAIL) {
        attron(COLOR_PAIR(3));
        mvprintw(17, 40, "%s", "YOU FAILED!!!!!");
        attroff(COLOR_PAIR(3));
        status = Status::NORMAL;
    }

#if 0
    mvprintw(0, 40, "first: %d, second: %d", curLocal.first, curLocal.second);
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            mvprintw(i + 1, j * 2 + 40, "%d", grid[i][j]);
            mvprintw(i + 1, j * 2 + 60, "%d", hole[i][j]);
            mvprintw(i + 1, j * 2 + 80, "%d", game[i][j]);
        }
    }
#else
    mvprintw(1, 40, "W: %s", "Move Up");
    mvprintw(3, 40, "S: %s", "Move Down");
    mvprintw(5, 40, "A: %s", "Move Right");
    mvprintw(7, 40, "D: %s", "Move Left");
    mvprintw(9, 40, "C: %s", "Inspection Results");
    mvprintw(11, 40, "R: %s", "Restart the current game");
    mvprintw(13, 40, "Q: %s", "Quit the current game");
    mvprintw(15, 40, "N: %s", "Start a new game");
#endif

    move(curLocal.second * 2 + 1, curLocal.first * 4 + 2);
}

void Sudoku::processInput() {
input:

    char ch = getch();
    for (const auto& val : {'a', 'd', 'w', 's', 'q', 'c', 'r', 'n'}) {
        if (ch == val) {
            ch -= 32;
        }
    }

    if ('A' == ch && curLocal.first >= 1) {
        curLocal.first -= 1;
    } else if ('D' == ch && curLocal.first <= 7) {
        curLocal.first += 1;
    } else if ('W' == ch && curLocal.second >= 1) {
        curLocal.second -= 1;
    } else if ('S' == ch && curLocal.second <= 7) {
        curLocal.second += 1;
    } else if ('Q' == ch) {
        status = Status::QUIT;
    } else if ('R' == ch) {
        copyHoleToGame();
    }
    else if ('N' == ch) {
        clear();
        zeroData();
        srand((unsigned)time(NULL));
        while(!fillFrom(0, 1));
        digHole();
    }
    else if ('C' == ch)  {
        if (isOver()) {
            status = Status::WIN;
        } else {
            status = Status::FAIL;
        }
    }else if (isdigit(ch)) {
        int x0 = curLocal.first;
        int y0 = curLocal.second;
        if (hole[y0][x0] != 0) {
            goto input;
        } else {
            game[y0][x0] = ch - '0';
        }
    }
}

void Sudoku::start() {
    initialize();
    do {
        char str[] = "Welcom to Sudoku Game";
        char first[] = "Press 'y/Y' to select the first level of difficulty.";
        char second[] = "Press 'u/U' to select the second level of difficulty.";
        char third[] = "Press 'i/I' to select the third level of difficulty.";
        char fourth[] = "Press 'o/O' to select the fourth level of difficulty.";
        char fifth[] = "Press 'p/P' to select the fifth level of difficulty.";
        mvprintw(1, 1, "%s", str);
        mvprintw(3, 1, "%s", first);
        mvprintw(5, 1, "%s", second);
        mvprintw(7, 1, "%s", third);
        mvprintw(9, 1, "%s", fourth);
        mvprintw(11, 1, "%s", fifth);

        char ch = toupper(getch());

        if ('Q' == ch) {
            break;
        } else if ('Y' == ch) {
            setLevel(Level::FIRST);
        } else if ('U' == ch) {
            setLevel(Level::SECOND);
        } else if ('I' == ch) {
            setLevel(Level::THIRD);
        } else if ('O' == ch) {
            setLevel(Level::FOURTH);
        } else if ('P' == ch) {
            setLevel(Level::FIFTH);
        } else {
            continue;
        }

        newGame();

        do {
            drawWindow();
            processInput();
        } while(status != Status::QUIT);

        clear();
        refresh();
    }while (true);
    shutdown();
}
