
#include "cycles.h"


char player_name[] = "Round the Clock";

int xdir[] = { 0, 1, 0, -1 };
int ydir[] = { -1, 0, 1, 0 };

strategy_init() {}
strategy_finish() {}

strategy()
{
	int x,y,dir;
	int score,best;
	int i,j;

	GetInfo(&x, &y, &dir);

	best = 1000; /* bigger than biggest possible score */

	for (i=0;i<4;i++) {

		if (Look(i)) continue;

		score = 0;

		for (j=2;j<4;j++)
			score += Inquire(x+j*xdir[i], y+j*ydir[i]);

		if (score < best ) {
			best = score;
			dir = i;
		}
	}
	return(dir);
}
