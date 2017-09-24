/* Routines for using the timer.device:
   Timer_Open(), Timer_Post(), and Timer_Close().
*/

#include <exec/types.h>
#include <exec/ports.h>
#include <devices/timer.h>

#include "cycles.h"

TIMEREQ *Timer_Open(unit,tp)
ULONG unit;
PORT *tp;
{
	int error;
	TIMEREQ *tr;

	if (!tp) tp = CreatePort(0,0);
	if (!tp) return(0);


	tr = (TIMEREQ *) CreateExtIO(tp,sizeof(TIMEREQ));
	if (!tr) {
		printf("Can't create timer request\n");
		return(0);
	}

	error = OpenDevice("timer.device",unit,tr,0);
	if (error) {
		Timer_Close(tr);
		return(0);
	}

	return(tr);
}

/* close the timer device down */
void Timer_Close(tr)
TIMEREQ *tr;
{
	PORT *tp;

	if (tr) {
		tp = tr->tr_node.io_Message.mn_ReplyPort;
		if (tp) DeletePort(tp);
		CloseDevice(tr);
		DeleteExtIO(tr,sizeof(TIMEREQ));
	}
}

/* ask the timer to reply to our message after a certain time */
void Timer_Post(tr,secs,micro)
TIMEREQ *tr;
{
	tr->tr_node.io_Command 	= TR_ADDREQUEST;
	tr->tr_time.tv_secs     = secs;
	tr->tr_time.tv_micro    = micro;
	SendIO(tr);
}
