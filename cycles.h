
/*
 * here we define all the structures and constants used in the server
 */

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/nodes.h>
#include <exec/ports.h>
#include <devices/timer.h>
#include <libraries/dos.h>

/* first some abbreviations that I like */
typedef struct Node		NODE;
typedef struct MsgPort		PORT;
typedef struct Message		MSG;
typedef struct List		LIST;
typedef struct timerequest	TIMEREQ;

/* info to describe each cycle */
typedef struct {
	NODE	node;		/* to keep things organized	*/
	struct	Order *msg;	/* place for pending message    */
	int	status;		/* current status		*/
	int	x,y;		/* current position		*/
	int	dir;		/* current direction		*/
	int	colour;		/* your colour designation	*/
} CYCLE;

typedef struct {
	int	x;		/* x size of screen image */
	int	y;		/* y size of screen image */
	BYTE	*buf;		/* pointer to buffer area */
} IMAGE;

/* specification for the messages we expect to receive  */
typedef struct Order {
	MSG	msg;		/* Exec message structure  */
	int	order;		/* command to be performed */
	int	x,y;		/* parameters		   */
	int	dir;		/* direction		   */
	int	status;		/* your current status	   */
	CYCLE	*cycle;		/* pointer to my cycle structure */
	IMAGE	*table;		/* pointer to screen table */
} ORDER;

#define STATUS_NEED_NEW	0	/* no direction change specified yet	*/
#define STATUS_GOT_MOVE	1	/* direction change specified: will use */

#define STATUS_DEAD	100	/* dummy entry -- bigger entries mean dead */
#define STATUS_LOSER	101	/* you've crashed -- out of this game	*/
#define STATUS_WINNER	102	/* you're the only remaining bike	*/
#define STATUS_AWAITING 103	/* waiting for a new game		*/
#define STATUS_REMOVED	104	/* you are no longer in the game	*/

#define ORD_DIRECTION	0	/* cycle change direction next move	*/
#define ORD_AWAIT	1	/* reply when the next game has begun	*/
#define ORD_RETIRE	2	/* I'm leaving --- forget all about me	*/

#define TURN_LEFT(dir)  (((dir)-1)&3)
#define TURN_RIGHT(dir) (((dir)+1)&3)

#define DIR_UP		0	/* constants for directions */
#define DIR_RIGHT	1	/* these are guaranteed never to change */
#define DIR_DOWN	2	/* in future revisions so you can do    */
#define DIR_LEFT	3	/* things like adding 1 (mod 4) to turn */
				/* right, -1 to turn left...            */

#define MAX_COLOURS	5	/* maximum number of colours & players  */
#define X_SIZE		40	/* clients are not allowed to look at	*/
#define Y_SIZE		23	/* these as the server may be compiled	*/
				/* with different values.  The server	*/
				/* decides the size of the arena	*/

extern MSG  	*GetMsg();
extern PORT	*CreatePort();
extern void	*AllocMem();
extern void	*CreateExtIO();
extern TIMEREQ	*Timer_Open();
extern void	Timer_Close();
extern void	Timer_Post();

extern int	scores[];
