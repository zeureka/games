#pragma once

#include <vector>
#include <cctype>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <curses.h>
#include <utility>

const float Ratio = 0.2;

class Mine {
public:
    Mine(const int& row = 10, const int& col = 10);
    ~Mine() = default;
    void startGame();

private:
    enum class GameStatus : char {Over, Execut, Quit, Success};
    void initCurses();
    void shutdown();
    void initGraphValue(const int& row, const int& col);
    void setRC(const int& rc);
    void setRC(const int& row, const int& col);
    void drawch(const int& row, const int& col, const char& alpha);
    void drawBoard(const std::vector<std::vector<char>>& graph);
    void recursiveSearch(const int& row, const int& col);
    int getCurPosMineNums(const int& row, const int& col);
    void processInput();

private:
    int ROWS;
    int COLS;
    int mineNums;
    int nonBlankNums;
    std::pair<int,  int> currLocal;
    std::vector<std::vector<char>> realGraph;
    std::vector<std::vector<char>> showGraph;
    GameStatus status;
};
