//主函数

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <malloc.h>
#include <memory.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>
#include <termio.h>
#include <time.h>
#include "STATUS1.h"
#include "PRIOR3.h"
#include "COUNT5.h"
#include "CONFIG.h"
#include "hashValA.h"
#include "hashValB.h"
#include "hashValC.h"
#include "Gomoku_main.h"
#include "Gomoku_hash.c"
#include "Gomoku_move.c"
#include "Gomoku_interface.c"

//--------------------
// 函数名称    |  main
// 函数功能    |  主函数
// 函数参数    |  无
// 函数返回值  |  无
//--------------------
int main()
{
 	char mode[100];
	stream = fopen("Gomoku.log", "w +");
	start();
	init();
	CreatBoard();
	CLEAR();
	MOVETO(1,1);
	do
	{
		printf("Gomoku -Develop by Li Fengzhi\n\nMode\n 1-Human VS Human 2-Human VS Computer\n");
		scanf("%s",mode);
		CLEAR();
		MOVETO(1,1);
	}while(strcmp(mode, "1") != 0 && strcmp(mode, "2") != 0);

	if (!strcmp(mode, "1"))
		man_to_man();//Human VS Human
	else
		man_to_computer();//Human VS Computer
	printf("Game Over\n");
	return 0;
}

//------------------------------------------
//--------Functions For Linux---------------
//------------------------------------------
int getch(void) //Get Char
{
     struct termios tm, tm_old;
     int fd = 0, ch;
     if (tcgetattr(fd, &tm) < 0) return -1;
     tm_old = tm;
     cfmakeraw(&tm);
     if (tcsetattr(fd, TCSANOW, &tm) < 0) return -1;
     ch = getchar();
     if (tcsetattr(fd, TCSANOW, &tm_old) < 0) return -1;
     return ch;
}

unsigned randomize(unsigned x) //Generate Random number (<x)
{
	return (int)(x * rand() / (RAND_MAX + 1.0));
}

ULONG GetTickCount() //Get Current Time (ms)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}
