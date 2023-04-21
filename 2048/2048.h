#ifndef _2048_H
#define _2048_H

#include <cstdlib>
#include <curses.h>
#include <iostream>
#include <string.h>
#include <string>
#include <vector>

#define W 5         // 格子的宽度
#define TARGET 2048 // 胜利条件

// 游戏状态  胜利 失败 游戏正常进行 退出
enum gameStatus { S_WIN, S_FAIT, S_NORMAL, S_QUIT };

void initialize();
void shutdown();

/****************************** Game Class **********************/
class Game {
private:
  int chessboardSize;                // 棋盘的大小
  gameStatus status;                 // 游戏状态
  std::vector<std::vector<int>> num; // 棋盘中每个位置的数字

public:
  Game() : status(S_NORMAL) {}
  ~Game() = default;
  void setChessboardSize(const int &size); // 设置格子的个数(4*4 or 5*5)
  void drawWindow();                       // 绘制游戏界面
  gameStatus getStatus() { return this->status; }; // 获取 status 的值
  void processInput(); // 处理按键（方向键）
  void start();        // 开始游戏

private:
  // 在指定的位置画一个字符
  void drawch(const int &row, const int &col, const char &c);
  // 游戏中的数字是右对齐， row，col 是数字最后一位所在的位置
  void drawNum(int row, int col, int val);
  // 重新开始游戏
  void restart();
  // 随机产生一个新的数字
  bool randNum();
  // 向左合并
  bool moveLeft();
  // 将数据成员数组逆时针旋转 90度
  // 如果是向上合并，可以先把数据成员数组逆时针旋转 90度，
  // 然后向左合并，再将数组逆时针旋转270度
  void rotate();
  // 判断游戏是否结束。还有空格或者相邻元素还能合并
  bool isOver();
};

#endif // !_2048_H
