#include <iostream>
#include <Windows.h>
#include <string>
#include <conio.h>
#include <graphics.h>
#include <stdlib.h>

using namespace std;

#define WINDOWS_LONG 1000		// 窗口长
#define WINDOWS_WIDE 700		// 窗口宽
#define MAP_X 11				// 地图 X
#define MAP_Y 15				// 地图 Y
#define GAMES 61				// 图片像素
#define OFFSET_X 42				// X 偏移量
#define OFFSET_Y 20				// Y 偏移量

#define Up 'w'
#define Down 's'
#define Left 'a'
#define Right 'd'
#define Exit 'q'

#define isCheek(pos) pos.x >= 0 && pos.x < MAP_X && pos.y >=0 && pos.y< MAP_Y		// 合法性检查

enum _MAP {
	WALL, 					// 墙
	FLOOR,					// 地板
	BOX_DESTINATION, 		// 箱子目的地
	MAN,					// 小人
	BOX,					// 箱子
	BOX_TRAGET,				// 箱子命中目标
	ALL						// 图数组
};

enum _KEY {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

// 小人的坐标位置
struct MAN {
	int x;
	int y;
};

struct MAN man;	// 定义小人坐标


// 游戏地图
// 墙：0;  地板：1;  箱子目的地：2;  小人：3;  箱子：4;  箱子命中目标：5
int map[MAP_X][MAP_Y] = {
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
{0, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 0},
{0, 1, 1, 1, 2, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0},
{0, 1, 1, 1, 1, 1, 1, 4, 1, 0, 2, 1, 1, 1, 0},
{0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0},
{0, 1, 1, 1, 1, 0, 1, 4, 1, 0, 2, 1, 1, 1, 0},
{0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0},
{0, 1, 1, 1, 2, 0, 1, 4, 1, 1, 1, 1, 1, 1, 0},
{0, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

IMAGE images[ALL];

// 显示步骤
void score(int s) {
	TCHAR str[16];

	putimage(OFFSET_X + 4 * GAMES, OFFSET_Y + 0 * GAMES, &images[WALL]);	// 更新数字墙壁
	putimage(OFFSET_X + 5 * GAMES, OFFSET_Y + 0 * GAMES, &images[WALL]);
	settextcolor(RGB(0, 255, 0));			// 设置字体颜色
	settextstyle(65, 0, _T("微软雅黑"));	// 设置字体的样式
	setbkmode(TRANSPARENT);					// 设置字体背景为透明
	outtextxy(42, 15, _T("剩余步骤："));	// 将字写入图像中
	_stprintf(str, _T("%d"), s);			// vc2010版本将整形转换为字符数组
	outtextxy(290, 20, str);				// 将转换后的数字写入图像中
}

// 步骤走完结束游戏
void end() {
	loadimage(0, _T("blackground.bmp"), WINDOWS_LONG, WINDOWS_WIDE, true);	// 游戏结束背景图
	settextcolor(RGB(255, 255, 255));										// 设置字体颜色
	settextstyle(50, 0, _T("微软雅黑"));									// 设置字体的样式
	setbkmode(TRANSPARENT);													// 设置字体背景为透明
	outtextxy(100, 300, _T("步骤走完，你不是一个合格的推箱子老司机！"));	// 将字写入图像中	
}

// 箱子都推完，游戏结束
void the_end() {
	loadimage(0, _T("blackground.bmp"), WINDOWS_LONG, WINDOWS_WIDE, true);	// 游戏结束背景图
	settextcolor(RGB(255, 255, 255));										// 设置字体颜色
	settextstyle(50, 0, _T("微软雅黑"));									// 设置字体的样式
	setbkmode(TRANSPARENT);													// 设置字体背景为透明
	outtextxy(110, 300, _T("恭喜您，成为了一名合格的推箱子老司机！"));
}

// 判断是否还有箱子目的地
bool the_end_decide() {
	for (int i=0; i<MAP_X; i++) {
		for (int j=0; j<MAP_Y; j++) {
			if (map[i][j] == BOX_DESTINATION) {
				return false;
			}
		}
	}

	return true;
}


// 位置互换
void is_go(struct MAN &XY, _MAP p) {
	map[XY.x][XY.y] = p;	// 更新地图
	putimage(OFFSET_X + XY.y * GAMES, OFFSET_Y + XY.x * GAMES, &images[p]);
}

// 实现小人行走和推箱子
void is_execute(enum _KEY key) {
	struct MAN next_pos = man;		// 小人前行一步
	struct MAN next_next_pos = man;	// 小人推箱子行走一步
	struct MAN flags = man;			// 标记小人站在箱子目的地的坐标
	static int flags_1 = 0;			// 小人可以路过箱子目的地的标记
	static int flags_2 = 0;			// 小人可以把箱子从箱子目的地推开
	static int flags_3 = 0;			// 小人站在箱子箱子目的地可以走开
	static int s = 100;

	switch (key) {
		case UP:
			next_pos.x--;
			next_next_pos.x -= 2;
			break;
		case DOWN:
			next_pos.x++;
			next_next_pos.x += 2;
			break;
		case LEFT:
			next_pos.y--;
			next_next_pos.y -= 2;
			break;
		case RIGHT:
			next_pos.y++;
			next_next_pos.y += 2;
			break;
	}
	
	if (s == 0) {
		end();
		system("pause");
		closegraph();		// 关闭图像
		exit(0);			// 退出程序
	}
	score(s);
	s--;

	if (isCheek(next_pos) && map[next_pos.x][next_pos.y] == FLOOR && flags_1 == 0) {	// 小人前面是地板
			is_go(next_pos, MAN);
			is_go(man, FLOOR);
			man = next_pos;
			if (flags_3 == 1) {		// 小人把箱子从箱子目的地推开后站在箱子目的地向地板前方走一步
				is_go(flags, BOX_DESTINATION);
				flags_2 = 1;
				flags_3 = 0;
			}
	} else if ((isCheek(next_pos) && (map[next_pos.x][next_pos.y] == BOX_DESTINATION) || map[next_pos.x][next_pos.y] == FLOOR)) {	// 小人前面是箱子目的地
		if (flags_1) {	// 小人可以穿过箱子目的地
			is_go(next_pos, MAN);
			is_go(man, BOX_DESTINATION);
			man = next_pos;
			flags_1 = 0;
		} else {
			is_go(next_pos, MAN);
			is_go(man, FLOOR);
			man = next_pos;
			flags_1 = 1;
		}
	} else if (isCheek(next_next_pos) && map[next_pos.x][next_pos.y] == BOX) {	// 小人前面是箱子
		if (map[next_next_pos.x][next_next_pos.y] == FLOOR) {	// 箱子前面是地板
				is_go(next_next_pos, BOX);
				is_go(man, FLOOR);
				is_go(next_pos, MAN);
				man = next_pos;
				if (flags_2 == 1) {		// 小人站在箱子目的地往前推箱子
					is_go(flags, BOX_DESTINATION);
					flags_2 = 0;
					flags_3 = 0;
				}
		} else if (map[next_next_pos.x][next_next_pos.y] == BOX_DESTINATION) {	// 箱子前方是箱子目的地
			is_go(next_next_pos, BOX_TRAGET);
			is_go(man, FLOOR);
			is_go(next_pos, MAN);
			man = next_pos;
		}
	} else if (isCheek(next_next_pos) && map[next_pos.x][next_pos.y] == BOX_TRAGET && map[next_next_pos.x][next_next_pos.y] == FLOOR) {	// 小人前面是箱子命中目标，箱子前面是地板
		is_go(next_pos, MAN);
		is_go(next_next_pos, BOX);
		is_go(man, FLOOR);
		man = next_pos;
		flags = next_pos;	// 获取当前箱子目的地的坐标
		flags_2 = 1;
		flags_3 = 1;
	}
}




int main(void) {
	IMAGE img;

	// 初始化窗口大小
	initgraph(WINDOWS_LONG, WINDOWS_WIDE);

	// 游戏背景图
	loadimage(&img, _T("blackground.bmp"), WINDOWS_LONG, WINDOWS_WIDE, true);
	putimage(0, 0, &img);

	// 初始化游戏背景
	loadimage(&images[WALL], _T("wall_right.bmp"), GAMES, GAMES, true);			// 0 ==> 墙
	loadimage(&images[FLOOR], _T("floor.bmp"), GAMES, GAMES, true);				// 1 ==> 地板
	loadimage(&images[BOX_DESTINATION], _T("des.bmp"), GAMES, GAMES, true);		// 2 ==> 箱子目的地
	loadimage(&images[MAN], _T("man.bmp"), GAMES, GAMES, true);					// 3 ==> 小人
	loadimage(&images[BOX], _T("box.bmp"), GAMES, GAMES, true);					// 4 ==> 箱子
	loadimage(&images[BOX_TRAGET], _T("box.bmp"), GAMES, GAMES, true);			// 5 ==> 箱子命中目标

	// 在窗口上显示地图
	for (int i=0; i<MAP_X; i++) {
		for (int j=0; j<MAP_Y; j++) {
			if (map[i][j] == 3) {		// 获取小人在地图中的坐标
				man.x = i;
				man.y = j;
			}
			putimage(OFFSET_X + j * GAMES, OFFSET_Y + i * GAMES, &images[map[i][j]]);
		}
	}


	// 小人行走
	bool flags = true;
	
	do {
		if (_kbhit()) {
			char key = _getch();

			if (key == Up) {
				is_execute(UP);
			} else if (key == Down) {
				is_execute(DOWN);
			} else if (key == Left) {
				is_execute(LEFT);
			} else if (key == Right) {
				is_execute(RIGHT);
			} else if (key == Exit) {
				flags = false;
			}
		}
		// 游戏结束判断
		if (the_end_decide()) {
			the_end();
			system("pause");
			closegraph();		// 关闭图像
			exit(0);			// 退出程序
		}

		Sleep(100);
	} while(flags == true);

	closegraph();
	return 0;
}


