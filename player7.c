
#include "cycles.h"

char player_name[] = "Look B4 U Leap";

int xdir[] = { 0, 1, 0, -1 };
int ydir[] = { -1, 0, 1, 0 };

static IMAGE myTable;
static nbytes;
static IMAGE *table = 0;
static x_bytes;

void mySet(), myClr();
char *malloc();
IMAGE *GetScreenMem();

strategy_init()
{
	table = GetScreenMem();

	myTable.x = table->x;
	myTable.y = table->y;
	x_bytes = (myTable.x>>3)+(!!(myTable.x&7));
	nbytes = myTable.y * x_bytes;
	myTable.buf = (BYTE *)malloc(nbytes);

	if (!myTable.buf) {
		printf("Error, can't get memory for my copy of the board\n");
		return(-1);
	}
	return(0);
}

strategy_finish()
{
	free(myTable.buf);
}

strategy()
{
	int x, y, dir;
	int score, best;
	int i, d;

	GetInfo(&x,&y,&dir);
	best = 0;
	copyBoard();
	for (d=dir, i=0; i<4; i++, d=TURN_RIGHT(d)) {
		score = value(x+xdir[d], y+ydir[d], 3);
		if (score > best) {
			dir = d;
			best = score;
		}
	}
	return(dir);
}

value(x, y, d)
{
	int i, best, v;

	if (myInquire(x, y)) return(0);


	if (d>0) {
		mySet(x,y);
		best = 0;
		for (i=0; i<4; i++) {
			v = value(x+xdir[i], y+ydir[i],d-1);
			if (v>best) best=v;
		}
		myClr(x, y);
		return(1+best);
	}
	return(1);
}

copyBoard()
{
	register i;
	register char *p, *q;

	q = myTable.buf;
	p = table->buf;

	for (i=nbytes; --i>=0; )
		*q++ = *p++;
}

static power[] = {1,2,4,8,16,32,64,128};

/* return the status of the pixel @ x,y */
myInquire(x,y)
{
	if (x<0 || y<0 || x>=myTable.x || y>=myTable.y) return(1);

	return ((myTable.buf[(x>>3) + y*x_bytes] & power[x&7]) != 0);
}

/* return the status of the pixel @ x,y */
void mySet(x,y)
{
	if (x<0 || y<0 || x>=myTable.x || y>=myTable.y) return;

	myTable.buf[(x>>3) + y*x_bytes] |= power[x&7];
}

/* return the status of the pixel @ x,y */
void myClr(x,y)
{
	if (x<0 || y<0 || x>=myTable.x || y>=myTable.y) return;

	myTable.buf[(x>>3) + y*x_bytes] &= ~power[x&7];
}
