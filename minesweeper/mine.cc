#include "mine.h"
#include <curses.h>
#include <vector>

Mine::Mine(const int& row, const int& col) : currLocal(std::make_pair(0, 0)), status(GameStatus::Execut) {
    initGraphValue(row, col);
    drawBoard(showGraph);
}

void Mine::startGame() {
    initCurses();

    do {
        drawBoard(showGraph);
        processInput();

        if (GameStatus::Over == status || GameStatus::Success == status) {
            drawBoard(realGraph);
            mvprintw(11, COLS * 2 + 5, "%s", "Game Over");
            char alpha = getch();
            if ('r' == alpha || 'R' == alpha) {
                initGraphValue(ROWS, COLS);
                status = GameStatus::Execut;
            } else if ('q' == alpha || 'R' == alpha) {
                status = GameStatus::Quit;
            }
        }

        if (GameStatus::Quit == status) {
            break;
        }
    } while (true);
    clear();
    shutdown();
}

void Mine::initCurses() {
    initscr(); // 初始化 curses
    cbreak();  // 不需要回车，直接交互
    noecho();  // 按键不显示
    start_color(); // 启用颜色显示
    curs_set(1);   // 显示光标
}

void Mine::shutdown() {
    endwin();
}

void Mine::initGraphValue(const int& row, const int& col) {
    ROWS = row;
    COLS = col;
    mineNums = ROWS * COLS * Ratio;
    nonBlankNums = ROWS * COLS;
    realGraph.resize(ROWS);
    showGraph.resize(ROWS);
    for (int i = 0; i < ROWS; ++i) {
        realGraph[i].resize(COLS);
        showGraph[i].resize(COLS);
    }

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            realGraph[i][j] = '0';
            showGraph[i][j] = '*';
        }
    }

    int nums = mineNums;
    int r = 0;
    int c = 0;
    srand((time(0)));

    while (nums > 0) {
        r = rand() % ROWS;
        c = rand() % COLS;

        if ('1' == realGraph[r][c]) {
            continue;
        }

        realGraph[r][c] = '1';
        --nums;
    }
}

void Mine::setRC(const int& rc) {
    initGraphValue(rc, rc);
}

void Mine::setRC(const int& row, const int& col) {
    initGraphValue(row, col);
}

void Mine::drawch(const int& row, const int& col, const char& alpha) {
    move(row, col);
    printw("%c", alpha);
}

void Mine::drawBoard(const std::vector<std::vector<char>>& graph) {
    clear();
    // 创建前景色与背景色键值对
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(12, COLOR_BLUE, COLOR_BLACK);
    init_pair(34, COLOR_YELLOW, COLOR_BLACK);
    init_pair(56, COLOR_GREEN, COLOR_BLACK);
    init_pair(78, COLOR_RED, COLOR_BLACK);

    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            int color = 0;
            if ('1' == graph[row][col] || '2' == graph[row][col]) {
                color = 12;
            } else if ('3' == graph[row][col] || '4' == graph[row][col]) {
                color = 34;
            } else if ('5' == graph[row][col] || '6' == graph[row][col]) {
                color = 56;
            } else if ('7' == graph[row][col] || '8' == graph[row][col]) {
                color = 78;
            }

            attron(COLOR_PAIR(color)); // 选用不同的颜色键值对
            drawch(row, col * 2, graph[row][col]);
            // drawch(row, col * 2 + 30, realGraph[row][col]);
            attroff(COLOR_PAIR(color));
        }
    }

    mvprintw(1, COLS * 2 + 5, "%s", "W/K: Up");
    mvprintw(2, COLS * 2 + 5, "%s", "S/J: Down");
    mvprintw(3, COLS * 2 + 5, "%s", "A/H: Left");
    mvprintw(4, COLS * 2 + 5, "%s", "D/L: Right");
    mvprintw(5, COLS * 2 + 5, "%s", "R  : Restart the game");
    mvprintw(6, COLS * 2 + 5, "%s", "Q  : Quit game");
    mvprintw(7, COLS * 2 + 5, "%s", "M  : Marking mine");
    mvprintw(8, COLS * 2 + 5, "%s", "U  : Unmark");
    mvprintw(10, COLS * 2 + 5,  "Current Location: (%d, %d)", currLocal.first, currLocal.second);
    // mvprintw(5, COLS * 2 + 5, "%d", nonBlankNums);
    move(currLocal.first, currLocal.second * 2);
}

void Mine::recursiveSearch(const int& row, const int& col) {
    // 递归结束条件：
    // 1.位置是否在有效范围内；不在有效范围内，则退出递归
    // 2.当前位置是否是空白格或数字；空白格或数字，则退出递归
    // 3.当前格子外围有地雷

    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
        return;
    }

    if ('*' != showGraph[row][col] && '$' != showGraph[row][col]) {
        return;
    }

    nonBlankNums -= 1;
    if (0 != getCurPosMineNums(row, col)) {
        showGraph[row][col] = getCurPosMineNums(row, col) + '0';
    } else {
        showGraph[row][col] = ' ';
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (0 == i && 0 == j) {
                    continue;
                }
                recursiveSearch(row + i, col + j);
            }
        }
    }

}

int Mine::getCurPosMineNums(const int& row, const int& col) {
    int count = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (0 == i && 0 == j) {
                continue;
            }

            if (row + i < 0 || row + i >= ROWS || col + j < 0 || col + j >= COLS) {
                continue;
            }

            if ('1' == realGraph[row + i][col + j]) {
                count += 1;
            }
        }
    }

    return count;
}

void Mine::processInput() {
input:
    char alpha = getch();
    alpha = toupper(alpha);

    if (GameStatus::Execut == status) {
        if (('A' == alpha || 'H' == alpha) && currLocal.second > 0) {
            currLocal.second -= 1;
        } else if (('D' == alpha || 'L' == alpha) && currLocal.second < COLS - 1) {
            currLocal.second += 1;
        } else if (('W' == alpha || 'K' == alpha) && currLocal.first > 0) {
            currLocal.first -= 1;
        } else if (('S' == alpha || 'J' == alpha) && currLocal.first < ROWS - 1) {
            currLocal.first += 1;
        } else if ('M' == alpha) {
            int row = currLocal.first, col = currLocal.second;
            if ('*' != showGraph[row][col]) {
                goto input;
            }
            showGraph[row][col] = '$';
        } else if ('U' == alpha) {
            int row = currLocal.first, col = currLocal.second;
            if ('$' != showGraph[row][col]) {
                goto input;
            }
            showGraph[row][col] = '*';
        } else if ('C' == alpha) {
            int row = currLocal.first, col = currLocal.second;
            if ('*' != showGraph[row][col] && '$' != showGraph[row][col]) {
                goto input;
            }

            if ('1' == realGraph[row][col]) {
                status = GameStatus::Over;
            } else {
                // 递归探索
                recursiveSearch(row, col);
            }
        }
    }

    if (nonBlankNums == mineNums) {
        status = GameStatus::Success;
    }

    if ('Q' == alpha) {
        status = GameStatus::Quit;
    } else if ('R' == alpha) {
        initGraphValue(ROWS, COLS);
    }
}

