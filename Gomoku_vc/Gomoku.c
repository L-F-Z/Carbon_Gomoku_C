#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <limits.h>
#include <malloc.h>
#include <memory.h>
#include <assert.h>
#include <math.h>
#include <windows.h>
#include "STATUS1.h"
#include "PRIOR3.h"
#include "COUNT5.h"
#include "CONFIG.h"
#include "hashValA.h"
#include "hashValB.h"
#include "hashValC.h"

typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned long ULONG;

#define SIZE 15
#define DEEP 5
#define WIDTH 8
#define OPPONENT(x) ((x) == OP ? XP : OP)
#define MAX_CELLS 625
#define MAX_CAND 256
#define TIMEOUT_PREVENT 5 //how much is minimax slower when the depth is increased
#define FOR_EVERY_CAND(x, y)                                    \
	for (y = upperLeftCand.cy; y <= lowerRightCand.cy; y++)     \
		for (x = upperLeftCand.cx; x <= lowerRightCand.cx; x++) \
			if (cell[x][y].piece == EMPTY && (cell[x][y].adj1 || cell[x][y].adj2))
#define ONE_CAND(plr, st)                                     \
	{                                                         \
		i = 0;                                                \
		while (cell[cnd[i].mx][cnd[i].my].status4[plr] != st) \
			i++;                                              \
		cnd[0] = cnd[i];                                      \
		*nCnd = 1;                                             \
		return;                                               \
	}
#define maximum(x, y) (((x) > (y)) ? (x) : (y))
#define minimum(x, y) (((x) < (y)) ? (x) : (y))
FILE *stream;

enum LNUM
{
	A = 8,
	B = 7,
	C = 6,
	D = 5,
	E = 4,
	F = 3,
	G = 2,
	H = 1,
	FORBID = 9
};
enum OXPiece
{
	EMPTY = 2,
	OP = 0,
	XP = 1,
	WRONG = 3
};

struct OXCell
{
	enum OXPiece piece;  //  状态信息 0为玩家1 1为玩家2 2为空 3为错误
	UCHAR pattern[4][2]; //  arrangement for 2 players and 4 directions
	UCHAR status1[4][2]; //  status of one direction
	UCHAR status4[2];	//  field status
	char adj1, adj2;	 //  does it have a neighbor within 1 and 2 fields
	UCHAR px, py;		 //  position of the cell
};
struct OXPoint
{
	UCHAR cx; // coordinate X
	UCHAR cy; // coordinate Y
};
struct OXMove
{
	//operator OXPoint() {return OXPoint(x, y);}
	short mx; //move X
	short my; //move Y
	int value;
};
struct HashRec
{
	ULONG hashB, hashC;
	short value;
	short depth;
	short moves;
	struct OXPoint best;
};

const char chess[3][3] = {"●", "◎", ""};
const int DX[4] = {1, 0, 1, 1};
const int DY[4] = {0, 1, 1, -1};
const int WIN_MIN = 25000;
const int WIN_MAX = 30000;
const int INF = 32000;

char board[SIZE + 1][SIZE * 2 + 10];	//输出棋盘
struct OXCell cell[SIZE + 8][SIZE + 8]; //数字化棋盘
int record[SIZE * SIZE + 2][2];			//记录每一步下的位置
int orderofplayer;						//0为玩家1先手   1为玩家1后手
int orderofcom;

//------------
int info_timeout_turn = 10000;	 /* time for one turn in milliseconds */
int info_max_memory = 350000000; /* maximum memory in bytes */
int info_exact5 = 0;			 //renju=0时，不限制 renju=1时,1为不限制，0为限制
int info_renju = 1;				 //0为不禁手 1为禁手
int terminateAI;

int nSt[2][10];	//nSt[i][j]-the number of the status j of the player i
int totalSearched; // the number of crawled items
int nSearched;	 // the number of crawled itemsf
int dep, tim;
enum OXPiece who, opp;
enum OXPiece firstPlayer;
unsigned int seed;							  //随机数种子
int moveCount;								  // the number of movements made
int remCount;								  // remCell length
struct OXPoint upperLeftCand, lowerRightCand; // the left-upward and right-lower-limit for search movements
int RANK[107];								  // points of configurations
int PRIOR[256][256];						  // priority
UCHAR STATUS4[10][10][10][10];				  // STATUS4, indexes 4 x status1
long start_time;
struct OXPoint choose;				 //解决参数传递问题
struct OXPoint remMove[MAX_CELLS];   // stores the next moves
struct OXCell *remCell[MAX_CELLS];   // stores the next moves
struct OXPoint remULCand[MAX_CELLS]; // stores the <upperLeftCand>
struct OXPoint remLRCand[MAX_CELLS]; // stores the <lowerRightCand>

struct HashRec table;
const ULONG hashValA[4096], hashValB[4096], hashValC[4096];
int info_max_memory;
struct HashRec *currentItem;
struct HashRec *elem;
ULONG hashASize, maxSize;
ULONG hashA, hashB, hashC;
signed char data[] = {
	15, 1, 4, 0, 0, 0, 1, 1, 2, 3, 2, 2, 4, 2, 5, 3, 3, 4, 3, 3, 5, 5, 4, 3, 3, 6, 4, 5, 6, 5, 5, 4, 3, 1,
	11, 1, 1, 1, 0, 0, 2, 2, 3, 2, 3, 3, 1, 3, 2, 4, 1, 5, 3, 3, 3, 2, 5, 1, 4, 3,
	11, 1, 1, 0, 0, 0, 1, 1, 3, 2, 2, 2, 2, 3, 3, 3, 2, 4, 2, 5, 1, 5, 1, 4, 0, 3,
	9, 1, 0, 0, 0, 3, 0, 1, 1, 2, 0, 2, 2, 3, 3, 3, 3, 4, 2, 4, 1, 1,
	9, 1, 1, 1, 0, 0, 2, 2, 3, 2, 3, 3, 2, 3, 1, 4, 1, 3, 1, 5, 2, 4,
	9, 1, 1, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 1, 2, 3, 1, 2, 0, 3, 1, 4,
	9, 1, 0, 0, 2, 0, 1, 0, 1, 1, 0, 1, 2, 2, 3, 2, 3, 3, 2, 3, 0, 2,
	9, 1, 1, 1, 0, 0, 2, 2, 0, 2, 2, 1, 3, 1, 1, 2, 1, 3, 0, 3, 3, 0,
	9, 1, 0, 0, 2, 0, 1, 1, 1, 2, 2, 1, 1, 4, 3, 2, 4, 3, 2, 3, 0, 1,
	8, 1, 0, 2, 1, 1, 1, 2, 2, 0, 2, 2, 3, 0, 3, 1, 3, 2, 4, 0,
	8, 1, 1, 1, 3, 0, 0, 2, 3, 1, 1, 3, 2, 2, 2, 3, 1, 4, 3, 3,
	7, 1, 0, 0, 1, 1, 0, 1, 2, 2, 3, 2, 3, 3, 2, 3, 0, 2,
	7, 1, 3, 2, 2, 1, 2, 2, 1, 1, 1, 0, 0, 0, 0, 1, 1, 2,
	7, 1, 0, 0, 0, 1, 1, 0, 0, 3, 2, 1, 3, 2, 1, 2, 1, -1,
	7, 1, 1, 0, 0, 1, 0, 2, 2, 2, 2, 1, 3, 2, 5, 2, 3, 0,
	7, 1, 0, 0, 0, 1, 2, 0, 0, 3, 2, 1, 1, 3, 1, 2, 2, 3,
	7, 1, 1, 0, 0, 0, 0, 1, 1, 1, 2, 2, 1, 2, 1, 3, 2, 3,
	7, 1, 0, 1, 1, 1, 2, 0, 0, 2, 1, 2, 3, 4, 2, 3, 2, 1,
	6, 1, 1, 0, 0, 0, 0, 2, 1, 1, 3, 3, 2, 2, 1, -1,
	6, 1, 1, 0, 0, 0, 1, 1, 2, 1, 2, 2, 1, 2, 0, -1,
	6, 1, 0, 0, 1, 0, 2, 1, 3, 1, 3, 2, 2, 2, 1, -1,
	6, 1, 3, 0, 0, 0, 1, 1, 2, 1, 2, 2, 1, 2, -1, 0,
	6, 1, 0, 0, 1, 1, 3, 1, 2, 2, 4, 3, 3, 3, 2, 4,
	6, 1, 0, 2, 0, 0, 1, 1, 1, 2, 2, 2, 2, 1, 3, 0,
	6, 1, 2, 1, 0, 0, 2, 2, 0, 1, 2, 3, 1, 2, 2, 0,
	6, 1, 2, 1, 0, 0, 1, 3, 0, 1, 2, 3, 1, 2, 2, 0,
	5, 2, 2, 2, 0, 0, 1, 0, 1, 1, 0, 1, 2, 1, 1, 2,
	5, 1, 0, 0, 2, 1, 0, 1, 2, 3, 1, 2, 1, 3,
	5, 1, 1, 0, 0, 0, 1, 1, 1, 2, 2, 1, 1, -1,
	5, 1, 1, 1, 1, 0, 0, 2, 1, 3, 1, 2, 2, 2,
	5, 1, 1, 0, 0, 0, 0, 1, 1, 1, 2, 3, 2, 2,
	5, 2, 0, 0, 1, 1, 1, 2, 0, 1, 2, 1, 2, 0, 3, 0,
	5, 1, 0, 2, 1, 0, 2, 0, 2, 1, 1, 1, -1, 3,
	5, 1, 1, 0, 1, 1, 0, 1, 2, 1, 3, 2, 2, -1,
	5, 1, 1, 1, 0, 0, 2, 1, 0, 2, 3, 1, 0, 1,
	5, 2, 1, 1, 0, 0, 2, 1, 1, 3, 1, 2, 0, 1, 3, 1,
	5, 2, 1, 0, 0, 0, 0, 1, 2, 1, 1, 1, 1, 2, -1, 2,
	5, 1, 1, 0, 1, 2, 0, 1, 3, 2, 2, 1, 2, -1,
	5, 2, 0, 1, 1, 2, 2, 1, 3, 0, 0, 3, 1, 1, 0, 2,
	5, 1, 2, 0, 0, 2, 1, 1, 1, 2, 2, 2, 0, 0,
	5, 1, 0, 0, 1, 1, 1, 0, 1, 2, 0, 2, 0, 1,
	4, 1, 1, 0, 0, 1, 1, 3, 1, 2, -1, 0,
	4, 1, 1, 0, 0, 1, 1, 1, 1, 2, -1, 0,
	4, 1, 1, 0, 1, 1, 0, 2, 2, 2, 0, 0,
	4, 1, 2, 0, 0, 0, 0, 2, 1, 1, 2, 2,
	4, 1, 0, 0, 1, 1, 3, 1, 2, 2, 3, 3,
	4, 2, 2, 0, 0, 0, 2, 2, 1, 1, 0, -1, 1, -1,
	4, 1, 2, 0, 0, 0, 2, 1, 1, 1, 2, 2,
	4, 1, 1, 0, 0, 1, 2, 0, 1, 2, -1, 0,
	4, 1, 1, 0, 0, 0, 0, 2, 1, 1, -1, -1,
	4, 1, 1, 0, 0, 1, 2, 2, 1, 2, -1, 0,
	4, 1, 0, 0, 1, 1, 2, 1, 2, 2, 3, 3,
	3, 1, 0, 1, 0, 0, 1, 0, 1, 1,
	3, 1, 0, 0, 2, 0, 1, 1, 2, 2,
	3, 4, 1, 0, 0, 1, 1, 2, 0, 2, 0, 0, 2, 0, 2, 2,
	3, 1, 1, 2, 0, 0, 1, 1, 1, 3,
	3, 1, 1, 0, 0, 0, 1, 1, 1, 2,
	3, 1, 0, 0, 0, 2, 0, 1, 0, -1,
	3, 1, 0, 0, 2, 1, 1, 1, 2, 2,
	3, 1, 0, 0, 0, 1, 1, 2, 1, 1,
	3, 2, 0, 0, 2, 2, 1, 1, 2, 1, 1, 2,
	3, 1, 0, 1, 3, 0, 2, 0, 1, 1,
	3, 1, 0, 0, 2, 1, 2, 2, 1, 1,
	3, 4, 0, 0, 1, 1, 2, 2, 1, -1, -1, 1, 3, 1, 1, 3,
	3, 1, 0, 0, 0, 2, 1, 2, 1, 0,
	3, 2, 1, 0, 0, 3, 1, 2, 2, 3, 1, 1,
	3, 2, 0, 0, 0, 3, 0, 2, 1, 3, -1, 3,
	3, 1, 0, 0, 0, 3, 1, 2, 1, 1,
	3, 1, 1, 0, 0, 2, 1, 2, 1, 1,
	3, 1, 0, 0, 2, 1, 1, 2, 0, 2,
	3, 1, 0, 0, 2, 2, 1, 2, 0, 1,
	3, 1, 0, 0, 3, 3, 2, 2, 1, 1,
	3, 1, 0, 0, 3, 2, 2, 2, 1, 1,
	3, 1, 0, 0, 3, 1, 2, 2, 3, 3,
	3, 6, 0, 0, 1, 0, 2, 0, 1, 1, 1, -1, 0, 2, 0, -2, 2, 2, 2, -2,
	3, 3, 0, 0, 3, 2, 2, 1, 1, 0, 1, 1, 0, 1,
	2, 3, 0, 0, 1, 1, 0, 2, 2, 0, 1, 2,
	2, 8, 0, 0, 1, 0, -1, -1, 0, -1, 1, -1, 2, -1, -1, 1, 0, 1, 1, 1, 2, 1,
	2, 2, 0, 0, 2, 2, 1, 3, 3, 1,
	1, 8, 0, 0, -1, 0, 1, 0, 0, 1, 0, -1, 1, 1, -1, 1, 1, -1, -1, -1,
	0, 0};
//------------

int isdight(char);
int issupper(char);
void man_to_man();		   //人人模式
void man_to_computer();	//人机模式
void CreatBoard();		   //绘制初始棋盘
void PrintBoard();		   //输出会自动更新数字化棋盘转化为的输出棋盘
int Ifwin(int);			   //判断是否有人获胜  0无人获胜  1有人获胜
int IfInBoard(int, int);   //判断落子是否在棋盘内  0不在棋盘内 1在棋盘内
void playerchess(int *);   //人机模式下玩家1下子
void comchess(int *);	  //人机模式下电脑下子
void opening(int *, int , int );
void setOpening(int *);
void gotoxy(int x,int y);
//-------------
void start(); //初始化
void init();  //初始化
void update1(int, int, int);
void update4(int, int);
void randomize();
unsigned random(unsigned);
UCHAR getStatus4(UCHAR, UCHAR, UCHAR, UCHAR);
int getRank(char);
int getPrior(UCHAR, UCHAR);
long stopTime();
void move(int, int);
void _move(int, int, int);
void setWho(enum OXPiece);
void initExact5();
void undo();
int undoxy(int, int);
void checkForbid(int, int);
int check();
int databaseMove(struct OXPoint *);
void yourTurn(int, int);
int evaluate();
void generateCand(struct OXMove *,int *); //OXCand
int quickWinSearch();
short prior(int, int);
struct OXMove minimax(int, int, int, int);

void HashClear();
int HashPresent();
void HashMove(int, int, enum OXPiece);
void HashUndo(int, int, enum OXPiece);
short HashValue();
short HashDepth();
short HashMoves();
struct OXPoint HashBest();
void HashUpdate(short, short, short, struct OXMove);
void HashResize(ULONG);
void HashInit();
void HashEnd();
//-------------

int main()
{
 	char mode[100];
	stream = fopen("Gomoku.log", "w +");
	start();
	init();
	CreatBoard();

	do
	{
		printf("五子棋 -Develop by LFZ\n\n请选择对战模式\n 1-双人对战 2-人机对战\n");
		scanf("%s",&mode);
		system("cls");
	}while(strcmp(mode, "1") != 0 && strcmp(mode, "2") != 0);

	if (!strcmp(mode, "1"))
		man_to_man();
	else
		man_to_computer();
	printf("游戏结束\n");
	system("pause");
	return 0;
}

//------------------------------
//------------HASH--------------
//------------------------------
void HashClear()
{
	hashASize = 0;
}

int HashPresent()
{
	return currentItem->hashB == hashB && currentItem->hashC == hashC;
}

void HashMove(int x, int y, enum OXPiece who)
{
	int offset = x + (y << 6) + (who << 11);
	hashA = (hashA + hashValA[offset]) % hashASize;
	currentItem = &elem[hashA];
	hashB = (hashB + hashValB[offset]);
	hashC = (hashC + hashValC[offset]);
}

void HashUndo(int x, int y, enum OXPiece who)
{
	int offset = x + (y << 6) + (who << 11);
	hashA = ((int)(hashA - hashValA[offset]) % (int)hashASize);
	if ((int)hashA < 0)
		hashA += hashASize;
	currentItem = &elem[hashA];
	hashB = (hashB - hashValB[offset]);
	hashC = (hashC - hashValC[offset]);
}

short HashValue()
{
	return currentItem->value;
}

short HashDepth()
{
	return currentItem->depth;
}

short HashMoves()
{
	return currentItem->moves;
}

struct OXPoint HashBest()
{
	return currentItem->best;
}

void HashUpdate(short _value, short _depth, short _moves, struct OXMove _best)
{
	struct HashRec *c = currentItem;
	c->value = _value;
	c->depth = _depth;
	c->moves = _moves;
	c->hashB = hashB;
	c->hashC = hashC;
	c->best.cx = _best.mx;
	c->best.cy = _best.my;
}

void HashResize(ULONG size)
{
	if (size > hashASize)
	{
		ULONG num;
		ULONG maxBytes = info_max_memory;
		if (maxBytes == 0)
			maxBytes = 1000000000; //1GB
		maxBytes = maximum(maxBytes, 7500000) - 7000000;
		num = maxBytes / sizeof(struct HashRec);
		if (maxSize * 2 < num || maxSize > num)
		{
			elem = (struct HashRec *)realloc(elem, maxBytes);
			maxSize = num;
		}
		if (hashASize < maxSize)
		{
			hashASize = minimum(size * 2, maxSize);
			memset(elem, 0, hashASize * sizeof(struct HashRec));
		}
	}
}

void HashInit()
{
	int n;
	maxSize = 0;
	hashASize = 0;
	elem = 0;
	for (n = 0; n < 4096; n++)
		assert(hashValA[n] < 2000000000);
}

void HashEnd()
{
	if (elem)
		free(elem);
}

//------------------------------
//------------HASH--------------
//------------------------------

void update1(int x, int y, int k)
{
	cell[x][y].status1[k][0] = STATUS1[cell[x][y].pattern[k][0]][cell[x][y].pattern[k][1]];
	cell[x][y].status1[k][1] = STATUS1[cell[x][y].pattern[k][1]][cell[x][y].pattern[k][0]];
}

void update4(int x, int y)
{
	cell[x][y].status4[0] = STATUS4[cell[x][y].status1[0][0]][cell[x][y].status1[1][0]][cell[x][y].status1[2][0]][cell[x][y].status1[3][0]];
	cell[x][y].status4[1] = STATUS4[cell[x][y].status1[0][1]][cell[x][y].status1[1][1]][cell[x][y].status1[2][1]][cell[x][y].status1[3][1]];
}

void start()
{
	int i, j, x, y, xx, yy, k;
	unsigned char p;
	randomize();
	for (x = 0; x < SIZE + 8; x++)
		for (y = 0; y < SIZE + 8; y++)
		{
			if (x < 4 || y < 4 || x >= SIZE + 4 || y >= SIZE + 4)
				cell[x][y].piece = WRONG;
			else
				cell[x][y].piece = EMPTY;
			for (k = 0; k < 4; k++)
				cell[x][y].pattern[k][0] = cell[x][y].pattern[k][1] = 0;
			cell[x][y].px = x;
			cell[x][y].py = y;
		}
	for (x = 4; x < SIZE + 4; x++)
		for (y = 4; y < SIZE + 4; y++)
			for (k = 0; k < 4; k++)
			{
				xx = x - DX[k];
				yy = y - DY[k];
				for (p = 8; p != 0; p >>= 1)
				{
					if (cell[xx][yy].piece == WRONG)
						cell[x][y].pattern[k][0] |= p;
					if (cell[xx][yy].piece == WRONG)
						cell[x][y].pattern[k][1] |= p;
					xx -= DX[k];
					yy -= DY[k];
				}
				xx = x + DX[k];
				yy = y + DY[k];
				for (p = 16; p != 0; p <<= 1)
				{
					if (cell[xx][yy].piece == WRONG)
						cell[x][y].pattern[k][0] |= p;
					if (cell[xx][yy].piece == WRONG)
						cell[x][y].pattern[k][1] |= p;
					xx += DX[k];
					yy += DY[k];
				}
			}
	for (x = 4; x < SIZE + 4; x++)
		for (y = 4; y < SIZE + 4; y++)
		{
			for (k = 0; k < 4; k++)
				update1(x, y, k);
				update4(x, y);
				cell[x][y].adj1 = cell[x][y].adj2 = 0;
		}
	for (i = 0; i < 2; i++)
		for (j = 0; j < 10; j++)
			nSt[i][j] = 0;
	totalSearched = 0;
	who = OP;
	opp = XP;
	moveCount = remCount = 0;
	upperLeftCand.cx = 99;
	upperLeftCand.cy = 99;
	lowerRightCand.cx = 0;
	lowerRightCand.cy = 0;
	HashInit();
	HashClear();
}

UCHAR getStatus4(UCHAR s0, UCHAR s1, UCHAR s2, UCHAR s3)
{
	int n[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	n[s0]++;
	n[s1]++;
	n[s2]++;
	n[s3]++;

	if (n[9] >= 1)
		return A; // OOOO_
	if (n[8] >= 1)
		return B; // OOO_
	if (n[7] >= 2)
		return B; // XOOO_ * _OOOX
	if (n[7] >= 1 && n[6] >= 1)
		return C; // XOOO_ * _OO
	if (n[7] >= 1 && n[5] >= 1)
		return D; // XOOO_ * _OOX
	if (n[7] >= 1 && n[4] >= 1)
		return D; // XOOO_ * _O
	if (n[7] >= 1)
		return E; // XOOO_
	if (n[6] >= 2)
		return F; // OO_ * _OO
	if (n[6] >= 1 && n[5] >= 1)
		return G; // OO_ * _OOX
	if (n[6] >= 1 && n[4] >= 1)
		return G; // OO_ * _O
	if (n[6] >= 1)
		return H; // OO_
	return 0;
}

int getRank(char cfg)
{
	int mul[5] = {3, 7, 11, 15, 19}; //0,1,4,9,16
	return mul[4] * COUNT5[cfg][4] +
		   mul[3] * COUNT5[cfg][3] +
		   mul[2] * COUNT5[cfg][2] +
		   mul[1] * COUNT5[cfg][1] +
		   mul[0] * COUNT5[cfg][0];
}

int getPrior(UCHAR a, UCHAR b)
{
	return _PRIOR[a][b];
}

void init()
{
	UCHAR a, b, c, d;
	for (a = 0; a < 10; a++)
		for (b = 0; b < 10; b++)
			for (c = 0; c < 10; c++)
				for (d = 0; d < 10; d++)
					STATUS4[a][b][c][d] = getStatus4(a, b, c, d);
	for (a = 0; a < 107; a++)
	{
		RANK[a] = getRank(a);
	}
	for (a = 0; a < 255; a++)
	{
		for (b = 0; b < 255; b++)
		{
			PRIOR[a][b] = getPrior(a, b);
		}
	}
	for (a = 0; a < 255; a++)
		PRIOR[a][255] = getPrior(a, 255);
	for (b = 0; b < 255; b++)
		PRIOR[255][b] = getPrior(255, b);
	PRIOR[255][255] = getPrior(255, 255);
}

long stopTime()
{
	return start_time + info_timeout_turn;
}

void move(int xp, int yp)
{
	HashResize(1);
	initExact5();
	_move(xp + 4, yp + 4, 1);
}

void setWho(enum OXPiece _who)
{
	who = _who;
	opp = OPPONENT(who);
	if (moveCount == 0)
		firstPlayer = _who;
}

void initExact5()
{
	int i, j, k;
	unsigned a, b, v;
	char y;
	static int last_exact5 = 0;
	if (info_exact5 == last_exact5)
		return;
	last_exact5 = info_exact5;
	y = info_exact5 ? 0 : 9;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 8; j++)
		{
			v = 0xf8 | j;
			a = ((v >> i) | (v << (8 - i))) & 0xff;
			for (k = 0; k < 8; k++)
			{
				b = ((k >> i) | (k << (8 - i))) & 0xff;
				assert(STATUS1[a][b] == 0 || STATUS1[a][b] == 9);
				STATUS1[a][b] = y;
			}
		}
	}
}

// ----------------------------------------------------------------------------
void _move(int xp, int yp, int updateHash)
{
	int x, y, k;
	UCHAR p;
	nSearched++;
	assert(check());
	nSt[0][cell[xp][yp].status4[0]]--;
	nSt[1][cell[xp][yp].status4[1]]--;

	cell[xp][yp].piece = who;
	remCell[remCount] = &cell[xp][yp];
	remMove[moveCount].cx = xp;
	remMove[moveCount].cy = yp;
	remULCand[remCount] = upperLeftCand;
	remLRCand[remCount] = lowerRightCand;
	moveCount++;
	remCount++;

	if (xp - 2 < upperLeftCand.cx)
		upperLeftCand.cx = maximum(xp - 2, 4);
	if (yp - 2 < upperLeftCand.cy)
		upperLeftCand.cy = maximum(yp - 2, 4);
	if (xp + 2 > lowerRightCand.cx)
		lowerRightCand.cx = minimum(xp + 2, SIZE + 3);
	if (yp + 2 > lowerRightCand.cy)
		lowerRightCand.cy = minimum(yp + 2, SIZE + 3);

	// modifying <pat> i <points>
	for (k = 0; k < 4; k++)
	{
		x = xp;
		y = yp;
		for (p = 16; p != 0; p <<= 1)
		{
			x -= DX[k];
			y -= DY[k];
			cell[x][y].pattern[k][who] |= p;
			if (cell[x][y].piece == EMPTY) // && (cell[x][y].adj1 || cell[x][y].adj2))
			{
				update1(x, y, k);
				nSt[0][cell[x][y].status4[0]]--;
				nSt[1][cell[x][y].status4[1]]--;
				update4(x, y);
				if (info_renju)
					checkForbid(x, y);
				nSt[0][cell[x][y].status4[0]]++;
				nSt[1][cell[x][y].status4[1]]++;
			}
		}
		x = xp;
		y = yp;
		for (p = 8; p != 0; p >>= 1)
		{
			x += DX[k];
			y += DY[k];
			cell[x][y].pattern[k][who] |= p;
			if (cell[x][y].piece == EMPTY) // && (cell[x][y].adj1 || cell[x][y].adj2))
			{
				update1(x, y, k);
				nSt[0][cell[x][y].status4[0]]--;
				nSt[1][cell[x][y].status4[1]]--;
				update4(x, y);
				if (info_renju)
					checkForbid(x, y);
				nSt[0][cell[x][y].status4[0]]++;
				nSt[1][cell[x][y].status4[1]]++;
			}
		}
	}

	// Add candidates
	cell[xp - 1][yp - 1].adj1++;
	cell[xp][yp - 1].adj1++;
	cell[xp + 1][yp - 1].adj1++;
	cell[xp - 1][yp].adj1++;
	cell[xp + 1][yp].adj1++;
	cell[xp - 1][yp + 1].adj1++;
	cell[xp][yp + 1].adj1++;
	cell[xp + 1][yp + 1].adj1++;
	cell[xp - 2][yp - 2].adj2++;
	cell[xp][yp - 2].adj2++;
	cell[xp + 2][yp - 2].adj2++;
	cell[xp - 2][yp].adj2++;
	cell[xp + 2][yp].adj2++;
	cell[xp - 2][yp + 2].adj2++;
	cell[xp][yp + 2].adj2++;
	cell[xp + 2][yp + 2].adj2++;

	// update the hash
	if (updateHash)
		HashMove(xp, yp, who);

	// players replacement
	who = OPPONENT(who);
	opp = OPPONENT(opp);
	assert(check());
}
// ----------------------------------------------------------------------------
void undo()
{
	int x, y, k;
	UCHAR p;
	int xp, yp;
	struct OXCell *c;
	assert(check());

	moveCount--;
	remCount--;
	xp = remMove[moveCount].cx;
	yp = remMove[moveCount].cy;
	upperLeftCand = remULCand[remCount];
	lowerRightCand = remLRCand[remCount];

	c = remCell[remCount];
	update1(c->px, c->py, 0);
	update1(c->px, c->py, 1);
	update1(c->px, c->py, 2);
	update1(c->px, c->py, 3);
	update4(c->px, c->py);
	if (info_renju)
		checkForbid(xp, yp);

	nSt[0][c->status4[0]]++;
	nSt[1][c->status4[1]]++;

	assert(c->piece == OP || c->piece == XP);
	c->piece = EMPTY;

	// players replacement
	who = OPPONENT(who);
	opp = OPPONENT(opp);

	// update the hash
	HashUndo(xp, yp, who);

	// modifying <pat>
	for (k = 0; k < 4; k++)
	{
		x = xp;
		y = yp;
		for (p = 16; p != 0; p <<= 1)
		{
			x -= DX[k];
			y -= DY[k];
			cell[x][y].pattern[k][who] ^= p;
			if (cell[x][y].piece == EMPTY) // && (cell[x][y].adj1 || cell[x][y].adj2))
			{
				update1(x, y, k);
				nSt[0][cell[x][y].status4[0]]--;
				nSt[1][cell[x][y].status4[1]]--;
				update4(x, y);
				if (info_renju)
					checkForbid(x, y);
				nSt[0][cell[x][y].status4[0]]++;
				nSt[1][cell[x][y].status4[1]]++;
			}
		}
		x = xp;
		y = yp;
		for (p = 8; p != 0; p >>= 1)
		{
			x += DX[k];
			y += DY[k];
			cell[x][y].pattern[k][who] ^= p;
			if (cell[x][y].piece == EMPTY) // && (cell[x][y].adj1 || cell[x][y].adj2))
			{
				update1(x, y, k);
				nSt[0][cell[x][y].status4[0]]--;
				nSt[1][cell[x][y].status4[1]]--;
				update4(x, y);
				if (info_renju)
					checkForbid(x, y);
				nSt[0][cell[x][y].status4[0]]++;
				nSt[1][cell[x][y].status4[1]]++;
			}
		}
	}

	// remove candidates
	cell[xp - 1][yp - 1].adj1--;
	cell[xp][yp - 1].adj1--;
	cell[xp + 1][yp - 1].adj1--;
	cell[xp - 1][yp].adj1--;
	cell[xp + 1][yp].adj1--;
	cell[xp - 1][yp + 1].adj1--;
	cell[xp][yp + 1].adj1--;
	cell[xp + 1][yp + 1].adj1--;
	cell[xp - 2][yp - 2].adj2--;
	cell[xp][yp - 2].adj2--;
	cell[xp + 2][yp - 2].adj2--;
	cell[xp - 2][yp].adj2--;
	cell[xp + 2][yp].adj2--;
	cell[xp - 2][yp + 2].adj2--;
	cell[xp][yp + 2].adj2--;
	cell[xp + 2][yp + 2].adj2--;

	assert(check());
}

int undoxy(int x, int y)
{
	if (moveCount > 0 && remMove[moveCount - 1].cx == x + 4 && remMove[moveCount - 1].cy == y + 4)
	{
		undo();
		return 0;
	}
	return 1;
}
// ----------------------------------------------------------------------------
void checkForbid(int x, int y)
{
	int k, n, s, x1, y1, n3, n4, n6;
	struct OXCell *c = &cell[x][y];
	if (c->status4[firstPlayer] < F)
		return;
	if (c->status4[firstPlayer] == A)
	{
		n6 = 0;
		for (k = 0; k < 4; k++)
		{
			if (c->status1[k][firstPlayer] >= 9)
			{
				n = -1;
				x1 = x;
				y1 = y;
				do
				{
					x1 -= DX[k];
					y1 -= DY[k];
					n++;
				} while (cell[x1][y1].piece == firstPlayer);
				x1 = x;
				y1 = y;
				do
				{
					x1 += DX[k];
					y1 += DY[k];
					n++;
				} while (cell[x1][y1].piece == firstPlayer);
				if (n >= 5)
				{
					if (n == 5)
						return; //five in a row
					n6++;
				}
			}
		}
		if (n6 > 0) //overline
			c->status4[firstPlayer] = FORBID;
		return;
	}

	n3 = n4 = 0;
	for (k = 0; k < 4; k++)
	{
		s = c->status1[k][firstPlayer];
		if (s >= 7)
			n4++;
		else if (s >= 6)
			n3++;
	}
	if (n4 > 1 || n3 > 1) //double-four or double-three
		c->status4[firstPlayer] = FORBID;
}
// ----------------------------------------------------------------------------
int check()
{
	int n[2][10];
	int i, j, x, y;
	for (i = 0; i <= 1; i++)
		for (j = 0; j < 10; j++)
			n[i][j] = 0;
	FOR_EVERY_CAND(x, y)
	{
		n[0][cell[x][y].status4[0]]++;
		n[1][cell[x][y].status4[1]]++;
	}
	for (i = 0; i < 2; i++)
		for (j = 1; j < 10; j++)
			if (n[i][j] != nSt[i][j])
			{
				return 0; //FALSE
			}
	return 1;			  //TRUE
}

// ----------------------------------------------------------------------------
int databaseMove(struct OXPoint *place)
{
	signed char *s, *sn;
	int i, x, y, x1, y1, flip, len1, len2, left, top, right, bottom;

	//board rectangle
	left = upperLeftCand.cx + 2;
	top = upperLeftCand.cy + 2;
	right = lowerRightCand.cx - 2;
	bottom = lowerRightCand.cy - 2;
	//find current board in the database
	for (s = data;; s = sn)
	{
		len1 = *s++;
		len2 = *s++;
		sn = s + 2 * (len1 + len2);
		if (len1 != moveCount)
		{
			if (len1 < moveCount)
				return 0; //FALSE:data must be sorted by moveCount descending
			continue;
		}
		//try all symmetries
		for (flip = 0; flip < 8; flip++)
		{
			for (i = 0;; i++)
			{
				x1 = s[2 * i];
				y1 = s[2 * i + 1];
				if (i == len1)
				{
					s += 2 * (len1 + random(len2));
					x1 = *s++;
					y1 = *s;
				}
				switch (flip)
				{
				case 0:
					x = left + x1;
					y = top + y1;
					break;
				case 1:
					x = right - x1;
					y = top + y1;
					break;
				case 2:
					x = left + x1;
					y = bottom - y1;
					break;
				case 3:
					x = right - x1;
					y = bottom - y1;
					break;
				case 4:
					x = left + y1;
					y = top + x1;
					break;
				case 5:
					x = right - y1;
					y = top + x1;
					break;
				case 6:
					x = left + y1;
					y = bottom - x1;
					break;
				default:
					x = right - y1;
					y = bottom - x1;
					break;
				}
				if (x - 4 < 0 || x - 4 >= SIZE || y - 4 < 0 || y - 4 >= SIZE)
					break;
				if (i == len1)
				{
					place->cx = x - 4;
					place->cy = y - 4;
					return 1; //TRUE
				}
				//compare current board and database
				if (cell[x][y].piece != ((i & 1) ? XP : OP))
					break;
			}
		}
	}
}

// ----------------------------------------------------------------------------
void yourTurn(int depth, int time)
{
	int prevSearched, turnSearched;
	long t0, t1, td;
	struct OXMove best;

	start_time = GetTickCount();
	terminateAI=0;
	turnSearched = 0;
	HashResize(50000);
	initExact5();

	if (moveCount == 0)
	{
		choose.cx = SIZE / 2;
		choose.cy = SIZE / 2;
		return;
	}

	if (databaseMove(&choose))
	{
		return;
	}

	if (time > 0)
		info_timeout_turn = time * 1000;

	if (depth > 0)
	{
		if (time == 0)
			info_timeout_turn = 1000000;
		nSearched = 0;
		best = minimax(depth, 1, -INF, INF);
		turnSearched = nSearched;
		choose.cx = best.mx - 4;
		choose.cy = best.my - 4;
	}
	else
	{
		prevSearched = 0;
		for (depth = 2; depth <= 50; depth++)
		{
			t0 = GetTickCount();

			nSearched = 0;
			best = minimax(depth, 1, -INF, INF);
			turnSearched += nSearched;

			if(terminateAI && depth > 4)
			{
				depth = 0; //timeout
				break;
			}

			choose.cx = best.mx - 4;
			choose.cy = best.my - 4;
			HashResize(nSearched * 2);

			t1 = GetTickCount();
			td = t1 - t0;
			if (terminateAI || t1 + TIMEOUT_PREVENT * td - stopTime() >= 0 || nSearched == prevSearched)
				break;
			prevSearched = nSearched;
		}
	}

	totalSearched += turnSearched;
	assert(!(choose.cx < 0 || choose.cx >= SIZE || choose.cy < 0 || choose.cy >= SIZE));
}
// ----------------------------------------------------------------------------
int evaluate()
{
	int a, i, k, p[2] = {0, 0};
	struct OXCell *c;
	for (i = 0; i < remCount; i++)
	{
		c = remCell[i]; 
		a = c->piece;
		for (k = 0; k < 4; k++)
		{
			p[a] += RANK[CONFIG[c->pattern[k][a]][c->pattern[k][1 - a]]];
		}
	}
	return p[who] - p[opp];
}
// ----------------------------------------------------------------------------
void generateCand(struct OXMove *cnd, int *nCnd) //OXCand
{
	int i, x, y;

	cnd[0].mx = -1;
	*nCnd = 0;
	// We set preconverter candidate, if there is a transposition in the table
	if (HashPresent() && HashDepth() >= 0 && HashBest().cx != 0)
	{
		cnd[0].mx = HashBest().cx;
		cnd[0].my = HashBest().cy;
		cnd[0].value = 10000;
		assert(cnd[0].mx != 0 && cnd[0].my != 0);
		assert(cell[cnd[0].mx][cnd[0].my].piece == EMPTY);
		*nCnd = 1;
	}

	// the rest of the candidates
	FOR_EVERY_CAND(x, y)
	if (x != cnd[0].mx || y != cnd[0].my)
	{
		cnd[*nCnd].mx = x;
		cnd[*nCnd].my = y;
		cnd[*nCnd].value = prior(x, y);
		if (cnd[*nCnd].value > 1)
			*nCnd+=1;//!!!!!!!!!!!!!!!! // Reject the bad possibilities
		assert(*nCnd < MAX_CAND);
	}
	if (nSt[who][A] > 0)
		ONE_CAND(who, A); // field of status
	if (nSt[opp][A] > 0)
		ONE_CAND(opp, A); // four blocking
	if (nSt[who][B] > 0)
		ONE_CAND(who, B); // creates no one around its four

	// blocking of the troika
	if (nSt[opp][B] > 0)
	{
		// restricts movements to
		*nCnd = 0;
		FOR_EVERY_CAND(x, y)
		if (cell[x][y].status4[who] >= E && cell[x][y].status4[who] != FORBID || cell[x][y].status4[opp] >= E && cell[x][y].status4[opp] != FORBID)
		{
			cnd[*nCnd].mx = x;
			cnd[*nCnd].my = y;
			cnd[*nCnd].value = prior(x, y);
			if (cnd[*nCnd].value > 0)
				*nCnd+=1;//!!!!!!!!!!!!!!!!
		}
		return;
	}
}
// ----------------------------------------------------------------------------
// Returns:
// 0-as usual
// positive-the number of movements to win
// negative-minus the number of movements to lose
int quickWinSearch()
{
	int x, y, q;
	if (nSt[who][A] >= 1)
		return 1; // there is a winning move
	if (nSt[opp][A] >= 2)
		return -2;		  // You can not block
	if (nSt[opp][A] == 1) // you need to block four
	{
		FOR_EVERY_CAND(x, y)
		if (cell[x][y].status4[opp] == A)
		{
			if (info_renju && who == firstPlayer && cell[x][y].status4[who] == FORBID)
				return -2; //cannot defend

			_move(x, y, 1);
			q = -quickWinSearch();
			undo();
			if (q < 0)
				q--;
			else if (q > 0)
				q++;
			return q;
		}
	}
	if (nSt[who][B] >= 1)
		return 3;		  // creates no one around its four
	if (nSt[who][C] >= 1) // XOOO_ * _OO
	{
		// Place the opponent only four saves spoiling the layout
		if (nSt[opp][B] == 0 && nSt[opp][C] == 0 && nSt[opp][D] == 0 && nSt[opp][E] == 0)
			return 5;
		// Analyze one more move
		FOR_EVERY_CAND(x, y)
		if (cell[x][y].status4[who] == C)
		{
			_move(x, y, 1);
			q = -quickWinSearch();
			undo();
			if (q > 0)
				return q + 1;
		}
	}
	if (nSt[who][F] >= 1)
	{
		// Place the opponent rescues four
		if (nSt[opp][B] == 0 && nSt[opp][C] == 0 && nSt[opp][D] == 0 && nSt[opp][E] == 0)
			return 5;
		// Analyze one more move - perhaps too big cost
		/*      FOR_EVERY_CAND(x, y)
        if (cell[x][y].status4[who] == F) 
          {
            _move(x, y, 1);
            q = -quickWinSearch();
            undo();
            if (q > 0) return q + 1;
          }*/
	}
	return 0;
}
// ----------------------------------------------------------------------------
// compared to qsort
int candComp(const void *a, const void *b)
{
	return ((struct OXMove *)b)->value - ((struct OXMove *)a)->value; //OXCand
}
// ----------------------------------------------------------------------------
short prior(int x, int y)
{
	return PRIOR[cell[x][y].pattern[0][0]][cell[x][y].pattern[0][1]] +
		   PRIOR[cell[x][y].pattern[1][0]][cell[x][y].pattern[1][1]] +
		   PRIOR[cell[x][y].pattern[2][0]][cell[x][y].pattern[2][1]] +
		   PRIOR[cell[x][y].pattern[3][0]][cell[x][y].pattern[3][1]] +
		   PRIOR[cell[x][y].pattern[0][1]][cell[x][y].pattern[0][0]] +
		   PRIOR[cell[x][y].pattern[1][1]][cell[x][y].pattern[1][0]] +
		   PRIOR[cell[x][y].pattern[2][1]][cell[x][y].pattern[2][0]] +
		   PRIOR[cell[x][y].pattern[3][1]][cell[x][y].pattern[3][0]] +
		   (cell[x][y].adj1 != 0);
}
// ----------------------------------------------------------------------------
// h - depth, h = 0 -> Returns the evaluation situation
// root - If this is the first call (true), or recursive (false)
// alpha - lower limit on the points of the searched movements
// beta  - upper bound
// Compartment <alpha, beta> It is closed.
struct OXMove minimax(int h, int root, int alpha, int beta)
{
	struct OXMove best, retu, m;
	struct OXMove cnd[MAX_CAND]; //OXCand // the candidate list of movements
	int i, x, y, value, q, nForbid, nCnd;
	short vA, vB;
	static int cnt;

	if (alpha > beta + 1)
	{
		retu.mx = 0;
		retu.my = 0;
		retu.value = beta + 1;
		return retu;
	}

	best.mx = 0;
	best.my = 0;
	best.value = alpha - 1;
	
	if(--cnt<0)
	{ 
		cnt=1000;  
		if(((long)GetTickCount()) - stopTime() > 0)
			terminateAI=2;
	}				
	// quickly identify Ending
	q = quickWinSearch();
	if (q != 0)
	{
		if (!root)
		{
			retu.mx = 0;
			retu.my = 0;
			retu.value = (q > 0 ? +WIN_MAX : -WIN_MAX) - q;
			return retu;
		}
		if (q == 1)
		{
			// traversing winning
			FOR_EVERY_CAND(x, y)
			if (cell[x][y].status4[who] == A)
			{
				retu.mx = x;
				retu.my = y;
				retu.value = WIN_MAX - 1;
				return retu;
			}
		}
	}
	// assessing the situation
	if (h == 0)
	{
		retu.mx = 0;
		retu.my = 0;
		retu.value = evaluate();
		return retu;
	}

	h--;
	// generating candidates
	generateCand(cnd, &nCnd);
	if (nCnd > 1)
	{
		qsort(cnd, nCnd, sizeof(struct OXMove), candComp); //OXCand
	}
	else if (nCnd == 1)
	{
		if (root)
		{
			retu.mx = cnd[0].mx;
			retu.my = cnd[0].my;
			retu.value = 0;
			return retu;
		}
	}
	else //(nCnd == 0)
	{
	noCand:
		// for almost full board, we do not reject any candidate
		FOR_EVERY_CAND(x, y)
		if (nCnd < MAX_CAND)
		{
			cnd[nCnd++].mx = x;
			cnd[nCnd++].my = y;
			cnd[nCnd++].value = 0;
		}
		if (nCnd == 0)
			best.value = 0; //board is full
	}

	nForbid = 0;
	// simulating movements
	for (i = 0; i < nCnd; i++)
	{
		if (info_renju && who == firstPlayer && cell[cnd[i].mx][cnd[i].my].status4[who] == FORBID)
		{
			nForbid++;
			continue;
		}

		HashMove(cnd[i].mx, cnd[i].my, who);
		assert(best.value <= beta);

		if (HashPresent() && (HashDepth() >= h && ((HashDepth() ^ h) & 1) == 0 || abs(HashValue()) >= WIN_MIN))
		{
			nSearched++;
			assert(HashMoves() == moveCount + 1);
			value = HashValue(); // This case is already calculated

			HashUndo(cnd[i].mx, cnd[i].my, who);
		}
		else
		{
			_move(cnd[i].mx, cnd[i].my, 0);

			// search window
			vA = -beta;				// No matter whether the movement has value <beta>, if +INF.
									// And so it will be rejected.
			vB = -(best.value + 1); // We want to movements better than the best so far.

			// Taking into account the widening of the window to change the winning score movements
			if (vB >= +WIN_MIN)
				vB++;
			if (vA <= -WIN_MIN)
				vA--;

			m = minimax(h, 0, vA, vB);
			value = -m.value;
			// the victory in k steps is better than k + 1 steps
			if (value >= +WIN_MIN)
				value--;
			if (value <= -WIN_MIN)
				value++;

			// the result is accurate only when located in a window
			if (-vB <= value && value <= -vA && !terminateAI)
				HashUpdate(value, h, moveCount, m);

			undo();
		}

		// update the best movement
		if (value > best.value)
		{
			best.mx = cnd[i].mx;
			best.my = cnd[i].my;
			best.value = value;
			if (value > beta)
			{
				retu.mx = best.mx;
				retu.my = best.my;
				retu.value = beta + 1;
				return retu;
			}
		}
		if(terminateAI)
			break;
	}

	if (info_renju && nForbid == nCnd)
	{
		nCnd = 0;
		goto noCand;
	}
	return best;
}
// ----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int isdight(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	else
		return -1;
}

int issupper(char c)
{
	if (c >= 'A' && c <= 'Z')
		return c - 'A' + 1;
	else
		return 0;
}

void CreatBoard() //绘制初始棋盘
{
	const char leftup[3] = "┏";
	const char up[3] = "┯";
	const char rightup[3] = "┓";
	const char left[3] = "┠";
	const char right[3] = "┨";
	const char leftdown[3] = "┗";
	const char down[3] = "┷";
	const char rightdown[3] = "┛";
	const char middle[3] = "┼";
	int i, j;

	memset(board, 0, sizeof board);
	board[1][1] = leftup[0];
	board[1][2] = leftup[1];
	for (j = 2; j <= SIZE - 1; j++)
	{
		board[1][2 * j - 1] = up[0];
		board[1][2 * j] = up[1];
	}
	board[1][2 * SIZE - 1] = rightup[0];
	board[1][2 * SIZE] = rightup[1];

	for (i = 2; i <= SIZE - 1; i++)
	{
		board[i][1] = left[0];
		board[i][2] = left[1];
		for (j = 2; j <= SIZE - 1; j++)
		{
			board[i][2 * j - 1] = middle[0];
			board[i][2 * j] = middle[1];
		}
		board[i][2 * SIZE - 1] = right[0];
		board[i][2 * SIZE] = right[1];
	}
	board[SIZE][1] = leftdown[0];
	board[SIZE][2] = leftdown[1];
	for (j = 2; j <= SIZE - 1; j++)
	{
		board[SIZE][2 * j - 1] = down[0];
		board[SIZE][2 * j] = down[1];
	}
	board[SIZE][2 * SIZE - 1] = rightdown[0];
	board[SIZE][2 * SIZE] = rightdown[1];
	return;
}

void PrintBoard() //输出会自动更新数字化棋盘转化为的输出棋盘
{
	int i, j;

	CreatBoard();
	for (i = 1; i <= SIZE; i++)
		for (j = 1; j <= SIZE; j++)
		{
			if (cell[i + 3][j + 3].piece == 0)
			{
				board[i][2 * j - 1] = chess[0][0];
				board[i][2 * j] = chess[0][1];
			}
			if (cell[i + 3][j + 3].piece == 1)
			{
				board[i][2 * j - 1] = chess[1][0];
				board[i][2 * j] = chess[1][1];
			}
		}

	for (i = 1; i <= SIZE; i++)
	{
		printf("%2d  ", SIZE - i + 1);
		for (j = 1; j <= 2 * SIZE; j++)
			printf("%c", board[i][j]);
		printf("\n");
	}
	printf("    ");
	for (i = 1; i <= SIZE; i++)
		printf("%c ", 'A' + i - 1);
	printf(" □←悔棋\n");
	return;
}

int IfInBoard(int x, int y) //判断落子是否在棋盘内  0不在棋盘内 1在棋盘内
{
	if (x < 1 || x > SIZE || y < 1 || y > SIZE)
		return 0;
	else
		return 1;
}

int Ifwin(int player) //判断是否有人获胜  0无人获胜  1有人获胜
{
	int i, j, len, mode, x, y;
	for (i = 1; i <= SIZE; i++)
		for (j = 1; j <= SIZE; j++)
			for (mode = 0; mode <= 3; mode++)
			{
				x = i;
				y = j;
				if (cell[x + 3][y + 3].piece != player)
					continue;
				len = 1;
				while (IfInBoard(x + DX[mode], y + DY[mode]) && cell[x + DX[mode] + 3][y + DY[mode] + 3].piece == player)
				{
					len++;
					if (len == 5)
						return 1;
					x += DX[mode];
					y += DY[mode];
				}
			}
	return 0;
}

int distance(int x1, int y1, int x2, int y2)
{
	return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

void gotoxy(int x,int y)   
{  
    COORD c;  
    c.X=x-1;  
    c.Y=y-1;  
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),c);  
} 

void playerchess(int *step) //人机模式下玩家1下子
{
	int nowx, nowy, ch;
	if (*step > 1)
	{
		printf("电脑下子的位置为：%d%c\n", SIZE - record[*step - 1][0], record[*step - 1][1] + 'A');
	}
	printf("玩家所用的棋子为%s\n电脑所用的棋子为%s\n", chess[orderofplayer], chess[orderofcom]);
	printf("请玩家选择落子位置或悔棋选项,回车确定");

	do
	{
		nowx = 6+record[*step-1][1]*2;
		nowy = record[*step-1][0]+1;
		gotoxy(nowx, nowy);
		if (*step == 1)
		{
			nowx = 20;
			nowy = 8;
			gotoxy(nowx, nowy);
		}
		while( (ch=getch())!=0x0d )
		{
			switch(ch)
			{
			case 0xE0:
				switch(ch=getch())
				{
					case 72:  nowy--; break;
					case 80:  nowy++; break;
					case 75:  nowx-=2; break;
					case 77:  nowx+=2; break;
					default: break;
				}
				break;
			default: break;
			}
			gotoxy(nowx,nowy);
		}
	}while (!(((nowx>4) && (nowx<=(4+2*SIZE)) && (nowy>=0) && (nowy<=SIZE) && (cell[nowy - 1+4][(nowx - 6) / 2+4].piece == EMPTY) && (cell[nowy - 1+4][(nowx - 6) / 2+4].status4[orderofplayer] != FORBID)) || ((nowx==36)&&(nowy==16))));
	if (nowx==36 && nowy==16)
	{
		undo();
		undo();
		*step-=3;
		return;
	}
	record[*step][0] = nowy - 1;
	record[*step][1] = (nowx - 6) / 2;
	move(record[*step][0], record[*step][1]);
	return;
}

void comchess(int *step) //人机模式下电脑下子
{
	yourTurn(dep,tim);
	record[*step][0] = choose.cx;//真实坐标
	record[*step][1] = choose.cy;//真实坐标
	move(choose.cx,choose.cy);
	return;
}

void opening(int *step, int x, int y)
{
	record[*step][0] = x;//真实坐标
	record[*step][1] = y;//真实坐标
	move(x, y);
	*step+=1;
}

void setOpening(int *step)
{
	int num;
	char code[100];
	short op[26][6]={{7,7,6,7,5,7},{7,7,6,7,5,8},{7,7,6,7,5,9},{7,7,6,7,6,8},
					 {7,7,6,7,6,9},{7,7,6,7,7,8},{7,7,6,7,7,9},{7,7,6,7,8,7},
					 {7,7,6,7,8,8},{7,7,6,7,8,9},{7,7,6,7,9,7},{7,7,6,7,9,8},
					 {7,7,6,7,9,9},{7,7,6,8,5,9},{7,7,6,8,6,9},{7,7,6,8,7,9},
					 {7,7,6,8,8,9},{7,7,6,8,9,9},{7,7,6,8,7,8},{7,7,6,8,8,8},
					 {7,7,6,8,9,8},{7,7,6,8,8,7},{7,7,6,8,9,7},{7,7,6,8,8,6},
					 {7,7,6,8,9,6},{7,7,6,8,9,5}};
	printf("人机对战模式\n\n请输入开局代码 D01-D13或I01-I13\n");
	scanf("%s",&code);
	while(!((code[0]=='D'||code[0]=='I') && ((code[1]=='0'&&code[2]>'0'&&code[2]<='9') || (code[1]=='1'&&code[2]>='0'&&code[2]<='3'))))
	{
		system("cls");
		printf("人机对战模式\n\n请输入开局代码 D01-D13或I01-I13\n");
		gets(code);
	}
	system("cls");
	if (code[0]=='D')
		num = (code[1]-'0')*10 + (code[2]-'0');
	else num = (code[1]-'0')*10 + (code[2]-'0') + 13;
	opening(step, op[num-1][0], op[num-1][1]);
	opening(step, op[num-1][2], op[num-1][3]);
	opening(step, op[num-1][4], op[num-1][5]);
}

void man_to_computer() //人机模式
{
	char first[100];
	int step = 1, c, i;
	long startTime;

	do
	{
		printf("人机对战模式\n\n请选择是否启用禁手\n 0-不启用 1-启用\n");
		scanf("%s",&first);
		system("cls");
	}while(strcmp(first, "0") != 0 && strcmp(first, "1") != 0);

	if (!strcmp(first, "0"))
		info_renju = 0;
	else
		info_renju = 1;
	system("cls");

	if (info_renju == 1)
	{
		do
		{
			printf("人机对战模式\n\n已开启三三禁手和四四禁手，是否启用长连禁手？\n 0-不启用 1-启用\n");
			scanf("%s",&first);
			system("cls");
		}while(strcmp(first, "0") != 0 && strcmp(first, "1") != 0);
		if (!strcmp(first, "0"))
			info_exact5 = 1;
		else
			info_exact5 = 0;
		system("cls");
	}

	do
	{
		printf("人机对战模式\n\n请设定搜索深度层数\n 请输入小于40的自然数 0为不限制\n");
		scanf("%d",&c);
		system("cls");
	}while((c < 0) || (c > 39));
	dep = c;
	system("cls");

	do
	{
		printf("人机对战模式\n\n请设定搜索时间限制(秒)\n 请输入不大于60的自然数 0为不限制\n");
		scanf("%d",&c);
		system("cls");
	}while((c < 0) || (c > 60));
	tim = c;
	system("cls");

	do
	{
		printf("人机对战模式\n\n请选择行棋顺序\n 1-玩家先手 2-电脑先手\n");
		scanf("%s",&first);
		system("cls");
	}while(strcmp(first, "1") != 0 && strcmp(first, "2") != 0);

	if (strcmp(first, "1") == 0)
	{
		orderofplayer = 0;
		orderofcom = 1;
		setWho(0);
	}
	else
	{
		orderofplayer = 1;
		orderofcom = 0;
		setWho(0);
	}
	system("cls");

	do
	{
		printf("人机对战模式\n\n请选择是否使用职业标准开局\n 1-使用 2-不使用\n");
		scanf("%s",&first);
		system("cls");
	}while(strcmp(first, "1") != 0 && strcmp(first, "2") != 0);

	if (strcmp(first, "1") == 0)
		setOpening(&step);
	
	PrintBoard();
	while (!Ifwin(step % 2) && step <= SIZE * SIZE)
	{
		if ((step + 1) % 2 == orderofplayer)
		{
			if (((strcmp(first, "1")==0)&&step>4)||((strcmp(first, "1")!=0)&&step>1))printf("电脑落子时间为%ld毫秒\n",(GetTickCount()-startTime));
			playerchess(&step);
		}
		else
		{
			startTime=GetTickCount();
			comchess(&step);
		}
		step++;
		system("cls");
		PrintBoard();
	}
	if (step > SIZE * SIZE)
		printf("棋盘已下满，请重新开始游戏\n");
	else if ((step % 2) == orderofplayer)
		printf("恭喜你赢了\n\n");
	else
		printf("抱歉你输了\n\n");

	printf("本次游戏双方一共进行了%d步\n", step - 1);

	fprintf(stream, "%d,%d,%d\n",orderofplayer+3,orderofcom+3,step-1);//player1,player2,step
	for (i = 1; i<step; i++)
	{
		fprintf(stream, "%d,%d\n",record[i][0]+1,record[i][1]+1);
	}
	return;
}

void man_to_man() //人人模式
{
	int step = 1; //正在进行的步数，奇数为玩家1 偶数为玩家2
	int nowx, nowy, ch, i;
	long startTime;
	while (!Ifwin(step % 2) && step <= SIZE * SIZE)
	{
		PrintBoard();
		if (step > 1)
			printf("上一位玩家下子的位置为：%d%c\n落子时间为%ld毫秒\n", SIZE - record[step - 1][0], record[step - 1][1] + 'A',(GetTickCount()-startTime));
		printf("玩家1所用的棋子为%s\n玩家2所用的棋子为%s\n请玩家选择落子位置或悔棋选项,回车确定",chess[0], chess[1]);
		startTime=GetTickCount();
		do
		{
			nowx = 6+record[step-1][1]*2;
			nowy = record[step-1][0]+1;
			gotoxy(nowx, nowy);
			if (step == 1)
			{
				nowx = 20;
				nowy = 8;
				gotoxy(nowx, nowy);
			}
			while( (ch=getch())!=0x0d )
			{
				switch(ch)
				{
				case 0xE0:
					switch(ch=getch())
					{
						case 72:  nowy--; break;
						case 80:  nowy++; break;
						case 75:  nowx-=2; break;
						case 77:  nowx+=2; break;
						default: break;
					}
					break;
				default: break;
				}
				gotoxy(nowx,nowy);
			}
		}while (!(((nowx>4) && (nowx<=(4+2*SIZE)) && (nowy>=0) && (nowy<=SIZE) && (cell[nowy - 1+4][(nowx - 6) / 2+4].piece == EMPTY) && (cell[nowy - 1+4][(nowx - 6) / 2+4].status4[orderofplayer] != FORBID)) || ((nowx==36)&&(nowy==16))));

		if (nowx==36 && nowy==16)
		{
			cell[record[step-1][0]+4][record[step-1][1]+4].piece = 2;
			step-=2;
			goto next;
		}

		record[step][0] = nowy - 1;
		record[step][1] = (nowx - 6) / 2;

		switch ((step + 1) % 2)
		{
		case 0:                                                                                                                                                                                                                                                                                                 
			cell[nowy - 1+4][(nowx - 6) / 2+4].piece = 0;
			break;
		case 1:
			cell[nowy - 1+4][(nowx - 6) / 2+4].piece = 1;
			break;
		default:
			break;
		}
		next:
		step++;
		system("cls");
	}
	if (step > SIZE * SIZE)
		printf("很遗憾，棋盘已下满，请重新开始游戏，最终棋盘如下：\n");
	else
		printf("恭喜玩家%d赢了，最终棋盘如下：\n", step % 2 +1);
	PrintBoard();
	printf("本次游戏双方一共进行了%d步\n", step - 1);
	fprintf(stream, "1,2,%d\n",step-1);//player1,player2,step
	for (i = 1; i<step; i++)
	{
		fprintf(stream, "%d,%d\n",record[i][0]+1,record[i][1]+1);
	}
	return;
}

void randomize()
{
	seed = GetTickCount();
}

unsigned random(unsigned x)
{
	seed = seed * 367413989 + 174680251;
	return (unsigned)(UInt32x32To64(x, seed) >> 32);
}