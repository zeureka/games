#include "2048.h"

void initialize() {
  initscr();   // ncurse初始化
  cbreak();    // 按键不需要输入回车直接交互
  noecho();    // 按键不显示
  curs_set(0); // 隐藏光标
}

void shutdown() {
  endwin(); // ncurses清理
}

/****************************** Game::drawch() **********************/
void Game::drawch(const int &row, const int &col, const char &c) {
  move(row, col);
  addch(c);
}

/****************************** Game::drawNum() **********************/
// 将数字写到通过 drawch() 写到屏幕上
void Game::drawNum(int row, int col, int val) {
  // 一位一位的写
  while (val > 0) {
    this->drawch(row, col, val % 10 + '0');
    val /= 10;
    --col;
  }
}

/****************************** Game::restart() **********************/
void Game::restart() {
  // 将数组的值全部赋为0
  for (int i = 0; i < this->chessboardSize; ++i) {
    for (int j = 0; j < this->chessboardSize; ++j) {
      this->num[i][j] = 0;
    }
  }

  this->randNum();
  this->randNum();         // 随机产生两个数字
  this->status = S_NORMAL; // 更改游戏状态
}

/****************************** Game::randNum() **********************/
bool Game::randNum() {
  // 用来记录屏幕上格子中为空的位置（即数组中值为零的位置）
  std::vector<int> emptyPos;
  for (int i = 0; i < this->chessboardSize; ++i) {
    for (int j = 0; j < this->chessboardSize; ++j) {
      if (0 == this->num[i][j]) {
        emptyPos.push_back(i * this->chessboardSize + j);
      }
    }
  }

  if (0 == emptyPos.size()) {
    return false;
  }

  // 随机找一个空的位置
  int index = emptyPos[rand() % emptyPos.size()];
  // 修改值
  this->num[index / this->chessboardSize][index % this->chessboardSize] =
      (rand() % 10 == 1) ? 4 : 2;
  return true;
}

/****************************** Game::moveLeft() **********************/
bool Game::moveLeft() {
  int tmp[this->chessboardSize][this->chessboardSize]; // 记录移动之前数组的值
  for (int i = 0; i < this->chessboardSize; ++i) {
    // 逐行处理
    int lastPos = 0;   // 记录合并数字的前一个数字的位置
    int lastValue = 0; // 记录合并数字的前一个数字的值

    for (int j = 0; j < this->chessboardSize; ++j) {
      tmp[i][j] = this->num[i][j];

      if (0 == this->num[i][j]) {
        continue; // 提前结束此次循环，因为当前位置为零，不能与上一个数字合并
      }

      if (0 == lastValue) {
        lastValue = this->num[i][j];
      } else {
        if (lastValue == this->num[i][j]) {
          // 如果 lastValue 的值与当前位置的值相同，则合并两个数字
          this->num[i][lastPos] = lastValue * 2;
          lastValue = 0; // 合并之后，将 lastValue 的值置为零,而不是更新为
                         // lastValue * 2，防止合并之后的数与下一个数再次合并

          if (TARGET == this->num[i][lastPos]) {
            this->status = S_WIN;
          }

        } else {
          // 将 lastValue 的值向前移动
          this->num[i][lastPos] = lastValue;
          // 更改 lastValue 的值为当前的值（当前的值肯定不为零）
          lastValue = this->num[i][j];
        }
        lastPos += 1; // 由于 lastPos 原位置上，不管当前的值是否于 lastValue
                      // 的值相等，都会被赋值，所以需要向后移动一位
      }
      // 原本的值要么向前移动了，要么合并了，所以要将此处的值赋为零
      this->num[i][j] = 0;
    }

    // 当前这一行已经遍历到最后一个位置了，lastValue 的值如果不为零，
    // 后面没有值可以和 lastValue 合并了，则将 lastValue 赋值到 lastPos 位置上
    if (0 != lastValue) {
      this->num[i][lastPos] = lastValue;
    }
  }
  // 判断执行移动操作之后的数组与之前的数组有没有变化，有变化则说明有移动或合并
  for (int i = 0; i < this->chessboardSize; ++i) {
    for (int j = 0; j < this->chessboardSize; ++j) {
      if (tmp[i][j] != num[i][j]) {
        return true;
      }
    }
  }
  return false;
}

/****************************** Game::rotate() **********************/
void Game::rotate() {
  // tmp 容器用来存放旋转后的数组的各个值
  int tmp[this->chessboardSize][this->chessboardSize];
  for (int i = 0; i < this->chessboardSize; ++i) {
    for (int j = 0; j < this->chessboardSize; ++j) {
      tmp[this->chessboardSize - i - 1][j] = this->num[j][i];
    }
  }

  for (int i = 0; i < this->chessboardSize; ++i) {
    for (int j = 0; j < this->chessboardSize; ++j) {
      this->num[i][j] = tmp[i][j];
    }
  }
}

/****************************** Game::isOver() **********************/
bool Game::isOver() {
  for (int i = 0; i < this->chessboardSize; ++i) {
    for (int j = 0; j < this->chessboardSize; ++j) {
      // 在竖直方向上还能进行合成或还有空格
      if (j + 1 < this->chessboardSize &&
          (this->num[i][j + 1] * this->num[i][j] == 0 ||
           this->num[i][j] == this->num[i][j + 1])) {
        return true;
      }

      // 在水平方向上还能进行合成或还有空格
      if (i + 1 < this->chessboardSize &&
          (this->num[i + 1][j] * this->num[i][j] == 0 ||
           this->num[i + 1][j] == this->num[i][j])) {
        return true;
      }
    }
  }
  return false;
}

/****************************** Game::processInput() **********************/
void Game::processInput() {
  char ch = getch(); // 获取用户的输入
  if (ch >= 'a' && ch <= 'z') {
    ch -= 32;
  }

  if (S_NORMAL == this->status) {
    bool updated = false; // 用来判断格子中的数字是否移动合并了

    if ('A' == ch) {
      updated = this->moveLeft();
    } else if ('D' == ch) {
      // 逆时针旋转 180度后进行左移，然后在逆时针旋转 180 度
      this->rotate();
      this->rotate();
      updated = this->moveLeft();
      this->rotate();
      this->rotate();
    } else if ('W' == ch) {
      // 逆时针旋转 90度后进行左移，然后在逆时针旋转 270度
      this->rotate();
      updated = this->moveLeft();
      this->rotate();
      this->rotate();
      this->rotate();
    } else if ('S' == ch) {
      // 逆时针旋转 270度后进行左移，然后在逆时针旋转 90度
      this->rotate();
      this->rotate();
      this->rotate();
      updated = this->moveLeft();
      this->rotate();
    }

    // 如果移动之后的数组有变化，则在数组中再随机生成一个数
    if (updated) {
      this->randNum();
    }

    if (!isOver()) {
      this->status = S_FAIT;
    }
  }

  if ('Q' == ch) {
    this->status = S_QUIT;
  } else if ('R' == ch) {
    this->restart();
  } else {
    return;
  }
}

/****************************** Game::chessboardSize() **********************/
void Game::setChessboardSize(const int &size) {
  this->chessboardSize = size;
  // 重新分配数组的大小
  this->num.resize(size, std::vector<int>(size));
  this->randNum();
  this->randNum();
}

/****************************** Game::drawWindow() **********************/
void Game::drawWindow() {
  clear(); // 清理屏幕
  int COLSLIDE = (COLS - this->chessboardSize * W - this->chessboardSize - 1) /
                 2; // 将界面向右滑行，使之居中
  for (int i = 0; i <= this->chessboardSize; ++i) {
    for (int j = 0; j <= this->chessboardSize; ++j) {
      // 绘制相交点
      this->drawch(i * 2, j * (W + 1) + COLSLIDE, '+');

      // 绘制竖线
      if (i != this->chessboardSize) {
        this->drawch(i * 2 + 1, j * (W + 1) + COLSLIDE, '|');
      }

      // 测试，方格内的数字
      if (i != this->chessboardSize && j != this->chessboardSize) {
        this->drawNum(i * 2 + 1, (j + 1) * (W + 1) - 1 + COLSLIDE,
                      this->num[i][j]);
      }

      // 绘制横线
      for (int k = 1; j != this->chessboardSize && k <= W; ++k) {
        this->drawch(i * 2, j * (W + 1) + k + COLSLIDE, '-');
      }
    }
  }
  char str[] = "w(Up), s(Down), a(Left), d(Right), R(Restart), Q(Quit)";
  mvprintw(2 * (this->chessboardSize + 1), (COLS - strlen(str)) / 2, "%s", str);

  if (S_WIN == this->status) {
    char str1[] = "YOU WIN, PRESS R TO CONTINUE OR Q QUIT";
    mvprintw(2 * (this->chessboardSize + 2), (COLS - strlen(str1)) / 2, "%s",
             str1);
  } else if (S_FAIT == this->status) {
    char str2[] = "YOU LOSE, PRESS R TO CONTINUE OR Q QUIT";
    mvprintw(2 * (this->chessboardSize + 2), (COLS - strlen(str2)) / 2, "%s",
             str2);
  }
}

/****************************** Game::start() **********************/
void Game::start() {
  initialize();
  do {
    char str[] = "Welcom to 2048 Game";
    char model1[] = "Press 'o/O' to select 4 * 4.";
    char model2[] = "Press 't/T' to select 5 * 5.";
    char quit[] = "Press 'q/Q' to exit the game.";
    mvprintw(1, (COLS - strlen(str)) / 2, "%s", str);
    mvprintw(3, (COLS - strlen(quit)) / 2, "%s", model1);
    mvprintw(5, (COLS - strlen(quit)) / 2, "%s", model2);
    mvprintw(7, (COLS - strlen(quit)) / 2, "%s", quit);
    char ch = getch();

    if ('q' == ch || 'Q' == ch) {
      break;
    }

    if ('o' == ch || 'O' == ch) {
      this->setChessboardSize(4);
      this->restart();
    } else if ('t' == ch || 'T' == ch) {
      this->setChessboardSize(5);
      this->restart();
    } else {
      continue;
    }

    do {
      this->drawWindow(); // 调用对象game的子函数绘制游戏界面
      this->processInput();
    } while (S_QUIT != this->getStatus()); // 当输入的字符为q或Q时退出游戏界面

    clear();
    refresh();

  } while (true);
  shutdown();
}
