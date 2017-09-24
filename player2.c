
#include "cycles.h"

char player_name[] = "Dizzy";

strategy_init() {}
strategy_finish() {}

strategy()
{
	int x,y,dir;

	GetInfo(&x, &y, &dir);

	if (!Look(TURN_LEFT(dir)))
		return(TURN_LEFT(dir));

	else if (!Look(TURN_RIGHT(dir)))
		return(TURN_RIGHT(dir));

	else
		return(dir);
}
