/* "play.c" - support routines for players:
   Register(), Retire(), Inquire(), GetScreenMem(), Direction(),
   Await(), GetInfo(), Look().
   
   These make it much easier to write a cycle player.
*/

#include "cycles.h"

PORT *FindPort();

static ORDER order;
static PORT *sPort;
static PORT *cPort;
static BYTE *screen;
static int  x_size,y_size,x_bytes;
static int  colour;

static xdir[] = {0,1,0,-1};	/* up, right, down, left */
static ydir[] = {-1,0,1,0};	/* up, right, down, left */

/* send a message to the target and wait for a reply */
MSG *Send(msg)
MSG *msg;
{
	PutMsg(sPort,msg);
	WaitPort(cPort);
	return(GetMsg(cPort));
}

/* register this player with the cycle server */
/* returns non zero if registration is unsuccessful */
/* also tells you which colour you will be playing (for output) */

Register(my_colour)
int *my_colour;
{

	cPort = CreatePort(0,0);
	if (!cPort) {
		printf("Can't create a reply port\n");
		return(-1);
	}

	order.msg.mn_Node.ln_Type = NT_MESSAGE;
	order.msg.mn_ReplyPort	  = cPort;
	order.order = ORD_AWAIT;
	order.cycle = 0;

	/* it is critical that the port not disappear between the time
	 * it is found and the first message is sent to it. 
	 * This will ensure that proper shutdown will always happen
	 * if the server decides to terminate
	 */

	Forbid();
	sPort = FindPort("Cycle Server");
	if (!sPort) {
		Permit();
		printf("Can't find Cycle Server Port\n");
		DeletePort(cPort);
		return(-1);
	}
	PutMsg(sPort,&order);
	Permit();

	WaitPort(cPort);
	GetMsg(cPort);

	if (order.status) {
		printf("Server shutting down.  No more clients accepted\n");
		DeletePort(cPort);
		return(-1);
	}

	colour  = order.cycle->colour;
	x_size  = order.table->x;
	y_size  = order.table->y;
	screen  = order.table->buf;
	x_bytes = (x_size>>3)+ !!(x_size & 7);
	*my_colour = colour;

	return(0);
}

/* retire from the game... free resources used to participate */
/* no diagnositics */

void Retire()
{
	/* check to see if we're already gone.  if so then no ORD_RETIRE */
	if (order.status != STATUS_REMOVED) {
	    order.order = ORD_RETIRE;
	    Send(&order);
	}
	DeletePort(cPort);
}


static int power[] = {1,2,4,8,16,32,64,128 };

/* return the status of the pixel @ x,y */
Inquire(x,y)
{
	if (x<0 || y<0 || x>=x_size || y>=y_size) return(1);

	return ((screen[(x>>3)+ y*x_bytes] & power[x&7]) != 0);
}

/* return the location of the screen... in case the client wishes
 * to do some custom testing
 */

IMAGE *GetScreenMem()
{
	return(order.table);
}

/* returns status from server, non-zero indicates something exceptional */
/* e.g. LOSER or WINNER declared */

Direction(dir)
{
	order.order = ORD_DIRECTION;
	order.dir = dir;
	Send(&order);
	return(order.status);
}

/* wait for new game to start */
Await()
{
	order.order = ORD_AWAIT;
	Send(&order);
	return(order.status);
}


/* get last known information about self from server */
GetInfo(x,y,dir)
int *x, *y, *dir;
{
	*x   = order.x;
	*y   = order.y;
	*dir = order.dir;
}

/* look in the given direction for a pixel */
Look(dir)
int dir;
{
	return (Inquire(order.x + xdir[dir], order.y + ydir[dir]));
}
