typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned long ULONG;

#define SIZE 15
#define MAX_CAND 256
#define MAX_CELLS 625
#define TIMEOUT_PREVENT 5
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
#define OPPONENT(x) ((x) == OP ? XP : OP)
#define CLEAR() printf("\033[2J")
#define MOVETO(x, y) printf("\033[%d;%dH", (x), (y))

FILE *stream;
enum LNUM {A = 8, B = 7, C = 6, D = 5, E = 4, F = 3, G = 2, H = 1, FORBID = 9};
enum OXPiece {EMPTY = 2, OP = 0, XP = 1, WRONG = 3};
struct OXCell
{
	enum OXPiece piece;  //  State 0=player1 1=player2 2=umpty 3=wrong
	UCHAR pattern[4][2]; //  situation for 2 players and 4 directions
	UCHAR status1[4][2]; //  status for 1 direction
	UCHAR status4[2];	 //  status for 4 direction
	char adj1, adj2;	 //  does it have a neighbor within 1 or 2 fields
	UCHAR px, py;		 //  position of the cell
};
struct OXPoint
{
	UCHAR cx; // coordinate X
	UCHAR cy; // coordinate Y
};
struct OXMove
{
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

const char chess[3][4] = {{0xEF,0xBF,0xAD}, {0xEF,0xBF,0xAE}, ""};
const int DX[4] = {1, 0, 1, 1};
const int DY[4] = {0, 1, 1,-1};
const int WIN_MIN = 25000;
const int WIN_MAX = 30000;
const int INF = 32000;

char board[SIZE + 1][SIZE * 6 + 10];	//output board
struct OXCell cell[SIZE + 8][SIZE + 8]; //digital borad
int record[SIZE * SIZE + 2][2];			//moves record
int orderofplayer;						//0=player_1_first   1=player_2_first
int orderofcom;

//------------
enum OXPiece who, opp;
enum OXPiece firstPlayer;
int info_timeout_turn = 10000;	 // time for one turn in milliseconds
int info_max_memory = 350000000; // maximum memory in bytes 
int info_exact5 = 0;			 //renju=0 not restricted renju=1,1=not restricted 0=restricted 
int info_renju = 1;				 //0=not restricted 1=restricted
int terminateAI;
int nSt[2][10];	//nSt[i][j]-the number of the status j of the player i
int totalSearched; // the number of crawled items
int nSearched;	 // the number of crawled itemsf
int dep, tim;
int moveCount;								  // the number of movements made
int remCount;								  // remCell length
struct OXPoint upperLeftCand, lowerRightCand; // the left-upward and right-lower-limit for search movements
int RANK[107];								  // points of configurations
int PRIOR[256][256];						  // priority
UCHAR STATUS4[10][10][10][10];				  // STATUS4, indexes 4 x status1
long start_time;
struct OXPoint choose;				 // pass the value
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

int getch(void);
void man_to_man();
void man_to_computer();
void CreatBoard();
void PrintBoard();
int Ifwin(int);
int IfInBoard(int, int);
void playerchess(int *);
void comchess(int *);
void opening(int *, int , int );
void setOpening(int *);
void start();
void init();
void update1(int, int, int);
void update4(int, int);
unsigned randomize(unsigned);
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
ULONG GetTickCount();