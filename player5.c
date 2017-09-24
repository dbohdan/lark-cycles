
#include "cycles.h"

int xdir[] = { 0, 1, 0, -1 };
int ydir[] = { -1, 0, 1, 0 };

char player_name[] = "Keep'm Flying";

strategy_init() {}
strategy_finish() {}

strategy()
{
	int x, y, dir;
	int score, best;
	int i, j, d;

	GetInfo(&x, &y, &dir);
	best = 8;  /* bigger than biggest possible score */
	for (d=dir, i=0; i<4; i++, d=TURN_RIGHT(d)) {
		if (Look(d)) continue;

		score = 0;
		for (j=2; j<4; j++)
			score += Inquire(x+j*xdir[d], y+j*ydir[d]);

		if (score < best ) {
			best = score;
			dir = d;
		}
	}
	return(dir);
}
