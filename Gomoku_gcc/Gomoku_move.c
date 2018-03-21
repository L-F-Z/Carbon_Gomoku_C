//核心行棋运算函数

//--------------------
// 函数名称    |  update1
// 函数功能    |  更新单方向评分值
// 函数参数    |  (x,y)为评分位置，k为方向
// 函数返回值  |  无
//--------------------
void update1(int x, int y, int k)
{
	cell[x][y].status1[k][0] = STATUS1[cell[x][y].pattern[k][0]][cell[x][y].pattern[k][1]];
	cell[x][y].status1[k][1] = STATUS1[cell[x][y].pattern[k][1]][cell[x][y].pattern[k][0]];
}

//--------------------
// 函数名称    |  update4
// 函数功能    |  更新多方向评分值
// 函数参数    |  (x,y)为评分位置
// 函数返回值  |  无
//--------------------
void update4(int x, int y)
{
	cell[x][y].status4[0] = STATUS4[cell[x][y].status1[0][0]][cell[x][y].status1[1][0]][cell[x][y].status1[2][0]][cell[x][y].status1[3][0]];
	cell[x][y].status4[1] = STATUS4[cell[x][y].status1[0][1]][cell[x][y].status1[1][1]][cell[x][y].status1[2][1]][cell[x][y].status1[3][1]];
}

//--------------------
// 函数名称    |  start
// 函数功能    |  初始化
// 函数参数    |  无
// 函数返回值  |  无
//--------------------
void start()
{
	int i, j, x, y, xx, yy, k;
	unsigned char p;
	srand((int)time(0));
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

//--------------------
// 函数名称    |  getStatus4
// 函数功能    |  获取方向状态值
// 函数参数    |  s0,s1,s2,s3
// 函数返回值  |  无
//--------------------
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

//--------------------
// 函数名称    |  getRank
// 函数功能    |  获取评分值
// 函数参数    |  cfg
// 函数返回值  |  无
//--------------------
int getRank(char cfg)
{
	int mul[5] = {3, 7, 11, 15, 19}; //0,1,4,9,16
	return mul[4] * COUNT5[cfg][4] +
		   mul[3] * COUNT5[cfg][3] +
		   mul[2] * COUNT5[cfg][2] +
		   mul[1] * COUNT5[cfg][1] +
		   mul[0] * COUNT5[cfg][0];
}

//--------------------
// 函数名称    |  getPrior
// 函数功能    |  获取主值
// 函数参数    |  a,b
// 函数返回值  |  无
//--------------------
int getPrior(UCHAR a, UCHAR b)
{
	return _PRIOR[a][b];
}

//--------------------
// 函数名称    |  init
// 函数功能    |  评分系统初始化
// 函数参数    |  无
// 函数返回值  |  无
//--------------------
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

//--------------------
// 函数名称    |  stopTime
// 函数功能    |  获取每一步应结束的绝对时间
// 函数参数    |  无
// 函数返回值  |  无
//--------------------
long stopTime()
{
	return start_time + info_timeout_turn;
}

//--------------------
// 函数名称    |  move
// 函数功能    |  在(xp,yp处落子)
// 函数参数    |  xp,yp
// 函数返回值  |  无
//--------------------
void move(int xp, int yp)
{
	HashResize(1);
	initExact5();
	_move(xp + 4, yp + 4, 1);
}

//--------------------
// 函数名称    |  setWho
// 函数功能    |  更新当前落子方
// 函数参数    |  _who为当前落子方
// 函数返回值  |  无
//--------------------
void setWho(enum OXPiece _who)
{
	who = _who;
	opp = OPPONENT(who);
	if (moveCount == 0)
		firstPlayer = _who;
}

//--------------------
// 函数名称    |  initExact5
// 函数功能    |  判断长连禁手
// 函数参数    |  无
// 函数返回值  |  无
//--------------------
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

//--------------------
// 函数名称    |  move
// 函数功能    |  主落子函数
// 函数参数    |  落子于(xp, yp)，updateHash = 1则更新hash
// 函数返回值  |  无
//--------------------
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

//--------------------
// 函数名称    |  undo
// 函数功能    |  悔棋主函数
// 函数参数    |  无
// 函数返回值  |  无
//--------------------
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

//--------------------
// 函数名称    |  undoxy
// 函数功能    |  悔棋函数
// 函数参数    |  (x, y)为悔棋位置
// 函数返回值  |  无
//--------------------
int undoxy(int x, int y)
{
	if (moveCount > 0 && remMove[moveCount - 1].cx == x + 4 && remMove[moveCount - 1].cy == y + 4)
	{
		undo();
		return 0;
	}
	return 1;
}

//--------------------
// 函数名称    |  checkForbid
// 函数功能    |  检查禁手函数
// 函数参数    |  (x,y)为被检测点
// 函数返回值  |  无
//--------------------
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

//--------------------
// 函数名称    |  check
// 函数功能    |  检查错误函数
// 函数参数    |  无
// 函数返回值  |  0-无错误 1-有错误
//--------------------
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

//--------------------
// 函数名称    |  databaseMove
// 函数功能    |  调用开局库
// 函数参数    |  place为被检测点
// 函数返回值  |  无
//--------------------
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
					s += 2 * (len1 + randomize(len2));
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

//--------------------
// 函数名称    |  yourTurn
// 函数功能    |  电脑落子运算主函数
// 函数参数    |  depth为搜索深度，mtime为限制时间
// 函数返回值  |  无
//--------------------
void yourTurn(int depth, int mtime)
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

	if (mtime > 0)
		info_timeout_turn = mtime * 1000;

	if (depth > 0)
	{
		if (mtime == 0)
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

//--------------------
// 函数名称    |  evaluate
// 函数功能    |  评分主函数
// 函数参数    |  无
// 函数返回值  |  无
//--------------------
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

//--------------------
// 函数名称    |  generateCand
// 函数功能    |  生成候选点
// 函数参数    |  cnd为候选点，nCnd为候选点个数
// 函数返回值  |  无
//--------------------
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
//--------------------
// 函数名称    |  generateCand
// 函数功能    |  生成候选点
// 函数参数    |  cnd为候选点，nCnd为候选点个数
// 函数返回值  |  0-正常进行运算
//               正数-距离胜利的步数
//  			 负数-举例失败的步数
//--------------------
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

//--------------------
// 函数名称    |  candComp
// 函数功能    |  评分比较函数
// 函数参数    |  a,b为被比较的两个候选点
// 函数返回值  |  无
//--------------------
int candComp(const void *a, const void *b)
{
	return ((struct OXMove *)b)->value - ((struct OXMove *)a)->value; //OXCand
}

//--------------------
// 函数名称    |  prior
// 函数功能    |  获取主值
// 函数参数    |  (x,y)
// 函数返回值  |  无
//--------------------
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

//--------------------
// 函数名称    |  minimax
// 函数功能    |  Minimax算法+AlphaBeta剪枝
//--------------------
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