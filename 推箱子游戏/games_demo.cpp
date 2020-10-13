#include <iostream>
#include <Windows.h>
#include <string>
#include <conio.h>
#include <graphics.h>
#include <stdlib.h>

using namespace std;

#define WINDOWS_LONG 1000		// ���ڳ�
#define WINDOWS_WIDE 700		// ���ڿ�
#define MAP_X 11				// ��ͼ X
#define MAP_Y 15				// ��ͼ Y
#define GAMES 61				// ͼƬ����
#define OFFSET_X 42				// X ƫ����
#define OFFSET_Y 20				// Y ƫ����

#define Up 'w'
#define Down 's'
#define Left 'a'
#define Right 'd'
#define Exit 'q'

#define isCheek(pos) pos.x >= 0 && pos.x < MAP_X && pos.y >=0 && pos.y< MAP_Y		// �Ϸ��Լ��

enum _MAP {
	WALL, 					// ǽ
	FLOOR,					// �ذ�
	BOX_DESTINATION, 		// ����Ŀ�ĵ�
	MAN,					// С��
	BOX,					// ����
	BOX_TRAGET,				// ��������Ŀ��
	ALL						// ͼ����
};

enum _KEY {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

// С�˵�����λ��
struct MAN {
	int x;
	int y;
};

struct MAN man;	// ����С������


// ��Ϸ��ͼ
// ǽ��0;  �ذ壺1;  ����Ŀ�ĵأ�2;  С�ˣ�3;  ���ӣ�4;  ��������Ŀ�꣺5
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

// ��ʾ����
void score(int s) {
	TCHAR str[16];

	putimage(OFFSET_X + 4 * GAMES, OFFSET_Y + 0 * GAMES, &images[WALL]);	// ��������ǽ��
	putimage(OFFSET_X + 5 * GAMES, OFFSET_Y + 0 * GAMES, &images[WALL]);
	settextcolor(RGB(0, 255, 0));			// ����������ɫ
	settextstyle(65, 0, _T("΢���ź�"));	// �����������ʽ
	setbkmode(TRANSPARENT);					// �������屳��Ϊ͸��
	outtextxy(42, 15, _T("ʣ�ಽ�裺"));	// ����д��ͼ����
	_stprintf(str, _T("%d"), s);			// vc2010�汾������ת��Ϊ�ַ�����
	outtextxy(290, 20, str);				// ��ת���������д��ͼ����
}

// �������������Ϸ
void end() {
	loadimage(0, _T("blackground.bmp"), WINDOWS_LONG, WINDOWS_WIDE, true);	// ��Ϸ��������ͼ
	settextcolor(RGB(255, 255, 255));										// ����������ɫ
	settextstyle(50, 0, _T("΢���ź�"));									// �����������ʽ
	setbkmode(TRANSPARENT);													// �������屳��Ϊ͸��
	outtextxy(100, 300, _T("�������꣬�㲻��һ���ϸ����������˾����"));	// ����д��ͼ����	
}

// ���Ӷ����꣬��Ϸ����
void the_end() {
	loadimage(0, _T("blackground.bmp"), WINDOWS_LONG, WINDOWS_WIDE, true);	// ��Ϸ��������ͼ
	settextcolor(RGB(255, 255, 255));										// ����������ɫ
	settextstyle(50, 0, _T("΢���ź�"));									// �����������ʽ
	setbkmode(TRANSPARENT);													// �������屳��Ϊ͸��
	outtextxy(110, 300, _T("��ϲ������Ϊ��һ���ϸ����������˾����"));
}

// �ж��Ƿ�������Ŀ�ĵ�
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


// λ�û���
void is_go(struct MAN &XY, _MAP p) {
	map[XY.x][XY.y] = p;	// ���µ�ͼ
	putimage(OFFSET_X + XY.y * GAMES, OFFSET_Y + XY.x * GAMES, &images[p]);
}

// ʵ��С�����ߺ�������
void is_execute(enum _KEY key) {
	struct MAN next_pos = man;		// С��ǰ��һ��
	struct MAN next_next_pos = man;	// С������������һ��
	struct MAN flags = man;			// ���С��վ������Ŀ�ĵص�����
	static int flags_1 = 0;			// С�˿���·������Ŀ�ĵصı��
	static int flags_2 = 0;			// С�˿��԰����Ӵ�����Ŀ�ĵ��ƿ�
	static int flags_3 = 0;			// С��վ����������Ŀ�ĵؿ����߿�
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
		closegraph();		// �ر�ͼ��
		exit(0);			// �˳�����
	}
	score(s);
	s--;

	if (isCheek(next_pos) && map[next_pos.x][next_pos.y] == FLOOR && flags_1 == 0) {	// С��ǰ���ǵذ�
			is_go(next_pos, MAN);
			is_go(man, FLOOR);
			man = next_pos;
			if (flags_3 == 1) {		// С�˰����Ӵ�����Ŀ�ĵ��ƿ���վ������Ŀ�ĵ���ذ�ǰ����һ��
				is_go(flags, BOX_DESTINATION);
				flags_2 = 1;
				flags_3 = 0;
			}
	} else if ((isCheek(next_pos) && (map[next_pos.x][next_pos.y] == BOX_DESTINATION) || map[next_pos.x][next_pos.y] == FLOOR)) {	// С��ǰ��������Ŀ�ĵ�
		if (flags_1) {	// С�˿��Դ�������Ŀ�ĵ�
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
	} else if (isCheek(next_next_pos) && map[next_pos.x][next_pos.y] == BOX) {	// С��ǰ��������
		if (map[next_next_pos.x][next_next_pos.y] == FLOOR) {	// ����ǰ���ǵذ�
				is_go(next_next_pos, BOX);
				is_go(man, FLOOR);
				is_go(next_pos, MAN);
				man = next_pos;
				if (flags_2 == 1) {		// С��վ������Ŀ�ĵ���ǰ������
					is_go(flags, BOX_DESTINATION);
					flags_2 = 0;
					flags_3 = 0;
				}
		} else if (map[next_next_pos.x][next_next_pos.y] == BOX_DESTINATION) {	// ����ǰ��������Ŀ�ĵ�
			is_go(next_next_pos, BOX_TRAGET);
			is_go(man, FLOOR);
			is_go(next_pos, MAN);
			man = next_pos;
		}
	} else if (isCheek(next_next_pos) && map[next_pos.x][next_pos.y] == BOX_TRAGET && map[next_next_pos.x][next_next_pos.y] == FLOOR) {	// С��ǰ������������Ŀ�꣬����ǰ���ǵذ�
		is_go(next_pos, MAN);
		is_go(next_next_pos, BOX);
		is_go(man, FLOOR);
		man = next_pos;
		flags = next_pos;	// ��ȡ��ǰ����Ŀ�ĵص�����
		flags_2 = 1;
		flags_3 = 1;
	}
}




int main(void) {
	IMAGE img;

	// ��ʼ�����ڴ�С
	initgraph(WINDOWS_LONG, WINDOWS_WIDE);

	// ��Ϸ����ͼ
	loadimage(&img, _T("blackground.bmp"), WINDOWS_LONG, WINDOWS_WIDE, true);
	putimage(0, 0, &img);

	// ��ʼ����Ϸ����
	loadimage(&images[WALL], _T("wall_right.bmp"), GAMES, GAMES, true);			// 0 ==> ǽ
	loadimage(&images[FLOOR], _T("floor.bmp"), GAMES, GAMES, true);				// 1 ==> �ذ�
	loadimage(&images[BOX_DESTINATION], _T("des.bmp"), GAMES, GAMES, true);		// 2 ==> ����Ŀ�ĵ�
	loadimage(&images[MAN], _T("man.bmp"), GAMES, GAMES, true);					// 3 ==> С��
	loadimage(&images[BOX], _T("box.bmp"), GAMES, GAMES, true);					// 4 ==> ����
	loadimage(&images[BOX_TRAGET], _T("box.bmp"), GAMES, GAMES, true);			// 5 ==> ��������Ŀ��

	// �ڴ�������ʾ��ͼ
	for (int i=0; i<MAP_X; i++) {
		for (int j=0; j<MAP_Y; j++) {
			if (map[i][j] == 3) {		// ��ȡС���ڵ�ͼ�е�����
				man.x = i;
				man.y = j;
			}
			putimage(OFFSET_X + j * GAMES, OFFSET_Y + i * GAMES, &images[map[i][j]]);
		}
	}


	// С������
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
		// ��Ϸ�����ж�
		if (the_end_decide()) {
			the_end();
			system("pause");
			closegraph();		// �ر�ͼ��
			exit(0);			// �˳�����
		}

		Sleep(100);
	} while(flags == true);

	closegraph();
	return 0;
}


