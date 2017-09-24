#include "cycles.h"

char player_name[] = "Mr. Right";

strategy_init() {}
strategy_finish() {}

strategy()
{
	int x,y,dir;

	GetInfo(&x,&y,&dir);
	if (!Look(dir))
		return(dir);

	if (!Look(TURN_RIGHT(dir)))
		return(TURN_RIGHT(dir));

	return(TURN_LEFT(dir));
}
