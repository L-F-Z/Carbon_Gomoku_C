//主界面函数

//--------------------
// 函数名称    |  CreatBoard
// 函数功能    |  产生空的字符棋盘用于打印
// 函数参数    |  无
// 函数返回值  |  无
//--------------------
void CreatBoard()
{
	const char leftup[7] = {0xE2,0x94,0x8C,0xE2,0x94,0x80};
	const char up[7] = {0xE2,0x94,0xAC,0xE2,0x94,0x80};
	const char rightup[7] = {0xE2,0x94,0x90,0x20,0x20,0x20};
	const char left[7] = {0xE2,0x94,0x9C,0xE2,0x94,0x80};
	const char right[7] = {0xE2,0x94,0xA4,0x20,0x20,0x20};
	const char leftdown[7] = {0xE2,0x94,0x94,0xE2,0x94,0x80};
	const char down[7] = {0xE2,0x94,0xB4,0xE2,0x94,0x80};
	const char rightdown[7] = {0xE2,0x94,0x98,0x20,0x20,0x20};
	const char middle[7] = {0xE2,0x94,0xBC,0xE2,0x94,0x80};
	int i, j, p;

	memset(board, 0, sizeof board);
	for (p = 0; p < 6; p++)//第一行赋值
		board[1][p + 1] = leftup[p];
	for (j = 2; j <= SIZE - 1; j++)
		for (p = 0; p < 6; p++)
			board[1][6 * j - 5 + p] = up[p];
	for (p = 0; p < 6; p++)
		board[1][6 * SIZE - 5 + p] = rightup[p];

	for (i = 2; i <= SIZE - 1; i++)//中间行赋值
	{
		for (p = 0; p < 6; p++)
			board[i][p + 1] = left[p];
		for (j = 2; j <= SIZE - 1; j++)
			for (p = 0; p < 6; p++)
				board[i][6 * j - 5 + p] = middle[p];
		for (p = 0; p < 6; p++)
		board[i][6 * SIZE - 5 + p] = right[p];
	}

	for (p = 0; p < 6; p++)//最后一行赋值
		board[SIZE][p + 1] = leftdown[p];
	for (j = 2; j <= SIZE - 1; j++)
		for (p = 0; p < 6; p++)
			board[SIZE][6 * j - 5 + p] = down[p];
	for (p = 0; p < 6; p++)
		board[SIZE][6 * SIZE - 5 + p] = rightdown[p];
	return;
}

//--------------------
// 函数名称    |  PrintBoard
// 函数功能    |  将数字棋盘的数据更新到字符棋盘，并在屏幕输出
// 函数参数    |  无
// 函数返回值  |  无
//--------------------
void PrintBoard()
{
	int i, j;

	CreatBoard();//产生空棋盘

	//更新下棋数据
	for (i = 1; i <= SIZE; i++)
		for (j = 1; j <= SIZE; j++)
		{
			if (cell[i + 3][j + 3].piece == 0)
			{
				board[i][6 * j - 5] = chess[0][0];
				board[i][6 * j - 4] = chess[0][1];
				board[i][6 * j - 3] = chess[0][2];
			}
			if (cell[i + 3][j + 3].piece == 1)
			{
				board[i][6 * j - 5] = chess[1][0];
				board[i][6 * j - 4] = chess[1][1];
				board[i][6 * j - 3] = chess[1][2];
			}
		}

	//打印
	for (i = 1; i <= SIZE; i++)
	{
		printf("%2d  ", SIZE - i + 1);
		for (j = 1; j <= 6 * SIZE; j++)
			printf("%c", board[i][j]);
		printf("\n");
	}
	printf("    ");
	for (i = 1; i <= SIZE; i++)
		printf("%c ", 'A' + i - 1);
	printf("□ ← Undo\n");
	return;
}

//--------------------
// 函数名称    |  IfInBoard
// 函数功能    |  判断某个点是否在棋盘之内
// 函数参数    |  x-点的x坐标值 y-点的y坐标值
// 函数返回值  |  0-不在棋盘中 1-在棋盘中
//--------------------
int IfInBoard(int x, int y)
{
	if (x < 1 || x > SIZE || y < 1 || y > SIZE)
		return 0;
	else
		return 1;
}

//--------------------
// 函数名称    |  Iswin
// 函数功能    |  判断某方是否胜利了
// 函数参数    |  player-选手编号
// 函数返回值  |  0-无胜利 1-有胜利
//--------------------
int Ifwin(int player)
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

//--------------------
// 函数名称    |  distance
// 函数功能    |  判断两点间相对距离的平方
// 函数参数    |  (x1,y1),(x2,y2)为两点的左边
// 函数返回值  |  两点间相对距离的平方
//--------------------
int distance(int x1, int y1, int x2, int y2)
{
	return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

//--------------------
// 函数名称    |  playerchess
// 函数功能    |  人机模式下玩家下子
// 函数参数    |  step为当前的总步数
// 函数返回值  |  无
//--------------------
void playerchess(int *step)
{
	int nowx, nowy, ch;
	if (*step > 1)
	{
		printf("Last Step: Computer - %d%c\n", SIZE - record[*step - 1][0], record[*step - 1][1] + 'A');
	}
	printf("Player   - %s\nComputer - %s\n", chess[orderofplayer], chess[orderofcom]);
	printf("Please choose your step or Undo, Press Enter to confirm");

	//扫描上下左右并更新光标位置
	do
	{
		nowx = record[*step-1][0]+1;
		nowy = 5+record[*step-1][1]*2;
		MOVETO(nowx, nowy);
		if (*step == 1)
		{
			nowx = 8;
			nowy = 19;
			MOVETO(nowx, nowy);
		}
		while( (ch=getch())!=0x0d)
		{
			switch(ch)
			{
			case 0x1B:
				switch(ch=getch())
				{
					case 0x5B:
						switch(ch=getch())
						{
							case 0x41: nowx--; break;//printf("↑"); break;
							case 0x42: nowx++; break;//printf("↓"); break;
							case 0x43: nowy+=2; break;//printf("→"); break;
							case 0x44: nowy-=2;//printf("←"); break;
							default: break;
						};
						MOVETO(nowx,nowy);
						break;
					default: break;
				}
				break;
			default: break;
			}
		}
	}while (!(((nowy>4) && (nowy<=(4+2*SIZE)) && (nowx>=0) && (nowx<=SIZE) && (cell[nowx - 1+4][(nowy - 5) / 2+4].piece == EMPTY) && (cell[nowx - 1+4][(nowy - 5) / 2+4].status4[orderofplayer] != FORBID)) || ((nowx==16)&&(nowy==35))));
	//悔棋
	if (nowx==16 && nowy==35)
	{
		undo();
		undo();
		*step-=3;
		return;
	}
	//记录落子位置
	record[*step][0] = nowx - 1;
	record[*step][1] = (nowy - 5) / 2;
	move(record[*step][0], record[*step][1]);
	return;
}

//--------------------
// 函数名称    |  comchess
// 函数功能    |  人机模式下电脑下子
// 函数参数    |  step为当前的总步数
// 函数返回值  |  无
//--------------------
void comchess(int *step)
{
	yourTurn(dep,tim);
	record[*step][0] = choose.cx;//real Coordinate
	record[*step][1] = choose.cy;//real Coordinate
	move(choose.cx,choose.cy);
	return;
}

//--------------------
// 函数名称    |  opening
// 函数功能    |  标准化开局落子
// 函数参数    |  step为当前的总步数，(x,y)为落子的坐标
// 函数返回值  |  无
//--------------------
void opening(int *step, int x, int y)
{
	record[*step][0] = x;//real Coordinate
	record[*step][1] = y;//real Coordinate
	move(x, y);
	*step+=1;
}

//--------------------
// 函数名称    |  setOpening
// 函数功能    |  标准化开局
// 函数参数    |  step为当前的总步数
// 函数返回值  |  无
//--------------------
void setOpening(int *step)
{
	int num;
	char code[100];
	//26种标准化开局
	short op[26][6]={{7,7,6,7,5,7},{7,7,6,7,5,8},{7,7,6,7,5,9},{7,7,6,7,6,8},
					 {7,7,6,7,6,9},{7,7,6,7,7,8},{7,7,6,7,7,9},{7,7,6,7,8,7},
					 {7,7,6,7,8,8},{7,7,6,7,8,9},{7,7,6,7,9,7},{7,7,6,7,9,8},
					 {7,7,6,7,9,9},{7,7,6,8,5,9},{7,7,6,8,6,9},{7,7,6,8,7,9},
					 {7,7,6,8,8,9},{7,7,6,8,9,9},{7,7,6,8,7,8},{7,7,6,8,8,8},
					 {7,7,6,8,9,8},{7,7,6,8,8,7},{7,7,6,8,9,7},{7,7,6,8,8,6},
					 {7,7,6,8,9,6},{7,7,6,8,9,5}};
	printf("Human VS Computer\n\nPlease Enter the opening code D01-D13 or I01-I13\n");
	scanf("%s",code);
	while(!((code[0]=='D'||code[0]=='I') && ((code[1]=='0'&&code[2]>'0'&&code[2]<='9') || (code[1]=='1'&&code[2]>='0'&&code[2]<='3'))))
	{
		CLEAR();
		MOVETO(1,1);
		printf("Human VS Computer\n\nPlease Enter the opening code D01-D13 or I01-I13\n");
		scanf("%s",code);
	}
	CLEAR();
	MOVETO(1,1);
	if (code[0]=='D')
		num = (code[1]-'0')*10 + (code[2]-'0');
	else num = (code[1]-'0')*10 + (code[2]-'0') + 13;
	opening(step, op[num-1][0], op[num-1][1]);
	opening(step, op[num-1][2], op[num-1][3]);
	opening(step, op[num-1][4], op[num-1][5]);
}

//--------------------
// 函数名称    |  man_to_computer
// 函数功能    |  人机对战主函数
// 函数参数    |  无
// 函数返回值  |  无
//--------------------
void man_to_computer()
{
	char first[100];
	int step = 1, c, i;
	long startTime;

	do
	{
		printf("Human VS Computer\n\njinshou\n 0-No 1-Yes\n");
		scanf("%s",first);
		CLEAR();
		MOVETO(1,1);
	}while(strcmp(first, "0") != 0 && strcmp(first, "1") != 0);
	
	//是否启用禁手
	if (!strcmp(first, "0"))
		info_renju = 0;
	else
		info_renju = 1;
	CLEAR();
	MOVETO(1,1);

	//是否启用长连禁手
	if (info_renju == 1)
	{
		do
		{
			printf("Human VS Computer\n\nchanglian\n 0-No 1-Yes\n");
			scanf("%s",first);
			CLEAR();
			MOVETO(1,1);
		}while(strcmp(first, "0") != 0 && strcmp(first, "1") != 0);
		if (!strcmp(first, "0"))
			info_exact5 = 1;
		else
			info_exact5 = 0;
		CLEAR();
		MOVETO(1,1);
	}

	//设定搜索层数
	do
	{
		printf("Human VS Computer\n\nPlease enter search DEPTH(0～40)，0-Unlimited\n");
		scanf("%d",&c);
		CLEAR();
		MOVETO(1,1);
	}while((c < 0) || (c > 39));
	dep = c;
	CLEAR();
	MOVETO(1,1);

	//设定时间限制
	do
	{
		printf("Human VS Computer\n\nPlease enter TIME limit(0～60)，0-Unlimited\n");
		scanf("%d",&c);
		CLEAR();
		MOVETO(1,1);
	}while((c < 0) || (c > 60));
	tim = c;
	CLEAR();
	MOVETO(1,1);

	//设定先后顺序
	do
	{
		printf("Human VS Computer\n\nPlease choose the order\n 1-Player First 2-Computer First\n");
		scanf("%s",first);
		CLEAR();
		MOVETO(1,1);
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
	CLEAR();
	MOVETO(1,1);

	//此版本中暂不进行设置
	// do
	// {
	// 	printf("Human VS Computer\n\nSet opening type?\n 1-YES 2-NO\n");
	// 	scanf("%s",first);
	// 	CLEAR();
	// 	MOVETO(1,1);
	// }while(strcmp(first, "1") != 0 && strcmp(first, "2") != 0);

	// if (strcmp(first, "1") == 0)
	// 	setOpening(&step);

	//轮流落子	
	PrintBoard();
	while (!Ifwin(step % 2) && step <= SIZE * SIZE)
	{
		if ((step + 1) % 2 == orderofplayer)
		{
			if (((strcmp(first, "1")==0)&&step>4)||((strcmp(first, "1")!=0)&&step>1))printf("Computer used %ld ms\n",(GetTickCount()-startTime));
			playerchess(&step);
		}
		else
		{
			startTime=GetTickCount();
			comchess(&step);
		}
		step++;
		CLEAR();
		MOVETO(1,1);
		PrintBoard();
	}
	if (step > SIZE * SIZE)
		printf("The board is full\n");
	else if ((step % 2) == orderofplayer)
		printf("Congratulations! You Win!\n\n");
	else
		printf("Sorry, you lost.\n\n");

	printf("Total Step: %d\n", step - 1);

	//log文件输出
	fprintf(stream, "%d,%d,%d\n",orderofplayer+3,orderofcom+3,step-1);//player1,player2,step
	for (i = 1; i<step; i++)
	{
		fprintf(stream, "%d,%d\n",record[i][0]+1,record[i][1]+1);
	}
	return;
}

//--------------------
// 函数名称    |  man_to_mam
// 函数功能    |  人人对战主函数
// 函数参数    |  无
// 函数返回值  |  无
//--------------------
void man_to_man() 
{
	int step = 1; //odd-player1 even-player2 
	int nowx, nowy, ch, i;
	long startTime;
	while (!Ifwin(step % 2) && step <= SIZE * SIZE)
	{
		PrintBoard();
		if (step > 1)
			printf("Last step - %d%c\nTime used - %ldms\n", SIZE - record[step - 1][0], record[step - 1][1] + 'A',(GetTickCount()-startTime));
		printf("Player 1 - %s\nPlayer 2 - %s\nPlease choose your step or Undo, Press Enter to confirm",chess[0], chess[1]);
		startTime=GetTickCount();
		//扫描落子位置
		do
		{
			nowx = record[step-1][0]+1;
			nowy = 5+record[step-1][1]*2;
			MOVETO(nowx, nowy);
			if (step == 1)
			{
				nowx = 8;
				nowy = 19;
				MOVETO(nowx, nowy);
			}
			while( (ch=getch())!=0x0d)
			{
				switch(ch)
				{
				case 0x1B:
					switch(ch=getch())
					{
						case 0x5B:
							switch(ch=getch())
							{
								case 0x41: nowx--; break;//printf("↑"); break;
								case 0x42: nowx++; break;//printf("↓"); break;
								case 0x43: nowy+=2; break;//printf("→"); break;
								case 0x44: nowy-=2;//printf("←"); break;
								default: break;
							};
							MOVETO(nowx,nowy);
							break;
						default: break;
					}
					break;
				default: break;
				}
			}
		}while (!(((nowy>4) && (nowy<=(4+2*SIZE)) && (nowx>=0) && (nowx<=SIZE) && (cell[nowx - 1+4][(nowy - 5) / 2+4].piece == EMPTY) && (cell[nowx - 1+4][(nowy - 5) / 2+4].status4[orderofplayer] != FORBID)) || ((nowx==16)&&(nowy==35))));
		if (nowx==16 && nowy==35)
		{
			cell[record[step-1][0]+4][record[step-1][1]+4].piece = 2;
			step-=2;
			goto next;
		}

		record[step][0] = nowx - 1;
		record[step][1] = (nowy - 5) / 2;

		switch ((step + 1) % 2)
		{
		case 0:                                                                                                                                                                                                                                                                                                 
			cell[nowx - 1+4][(nowy - 5) / 2+4].piece = 0;
			break;
		case 1:
			cell[nowx - 1+4][(nowy - 5) / 2+4].piece = 1;
			break;
		default:
			break;
		}
		next:
		step++;
		CLEAR();
		MOVETO(1,1);
	}
	if (step > SIZE * SIZE)
		printf("The Board is Full\n");
	else
		printf("Winner - %d Result:\n", step % 2 +1);
	PrintBoard();
	printf("Total Steps - %d\n", step - 1);
	fprintf(stream, "1,2,%d\n",step-1);//player1,player2,step
	for (i = 1; i<step; i++)
	{
		fprintf(stream, "%d,%d\n",record[i][0]+1,record[i][1]+1);
	}
	return;
}