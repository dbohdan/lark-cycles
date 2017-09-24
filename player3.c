
#include "cycles.h"

char player_name[] = "Vertigo";

strategy_init() {}
strategy_finish() {}

strategy()
{
	int x,y,dir;

	GetInfo(&x, &y, &dir);

	/* (dir & 1) is true for left and right */

	if (dir & 1) {
		if (!Look(TURN_RIGHT(dir)))
			return(TURN_RIGHT(dir));

		else if (!Look(TURN_LEFT(dir)))
			return(TURN_LEFT(dir));

		else
			return(dir);
	}
	else {
		if (!Look(dir))
			return(dir);

		else if (!Look(TURN_RIGHT(dir)))
			return(TURN_RIGHT(dir));

		else
			return(TURN_LEFT(dir));
	}
}
