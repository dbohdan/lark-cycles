/* "manager.c" - the mainline for the cycle players. 
   Puts up window, keeps score, etc.
*/

#include "cycles.h"
#include <intuition/intuition.h>

#define PER_ROW	3	/* windows per row */
#define HEIGHT   50	/* height of each window */

extern char player_name[];

static struct Window	   *war_window;
static struct RastPort	*rp;

struct IntuitionBase	*IntuitionBase;
struct GfxBase		   *GfxBase;

struct Window		   *OpenWindow();
APTR                 OpenLibrary();

int wins, losses;
int colour;

char *colours[] = { "Red", "Blue", "White", "Green", "Yellow" };

main()
{
	int status;
	if (Register(&colour)) exit(0);

	if (strategy_init()) {
		Retire();
		exit(0);
	}
	
	if (g_init((colour%PER_ROW)*(640/PER_ROW),(colour/PER_ROW)*HEIGHT)) {
		strategy_finish();
		Retire();
		exit(0);
	}

	g_show();

	status = STATUS_NEED_NEW;

	for (;;) {
		if (GetMsg(war_window->UserPort)) {
			strategy_finish();
			Retire();
			g_finish();
			exit(0);
		}

		switch (status) {

   		case STATUS_WINNER:
   			wins++;
   			g_show();
   			status = Await();
   			break;
   
   		case STATUS_LOSER:
   			losses++;
   			g_show();
   			status = Await();
   			break;
   
   		case STATUS_NEED_NEW:
   			status = strategy();
   			if (status < 0) {
   				strategy_finish();
   				Retire();
   				g_finish();
   				exit(0);
			}
			status = Direction(status);
			break;

		case STATUS_REMOVED:
			WaitPort(war_window->UserPort);
			GetMsg(war_window->UserPort);
			strategy_finish();
			Retire();
			g_finish();
			exit(0);
		}
	}
}


/*
 * here we have all of the graphics rendering code for the player...
 *
 */

static struct NewWindow newwindow = {
	0,0,			/* left edge, top edge */
	640/PER_ROW-2, HEIGHT-1,	/* width, height */
	0,1,			/* detail pen, block pen */
	CLOSEWINDOW,		/* IDCMP Flags*/
	 WINDOWCLOSE
	| NOCAREREFRESH
	| SMART_REFRESH
	| WINDOWDRAG
	| WINDOWDEPTH,	/* flags */
	0,			      /* first gadget */
	0,			      /* checkmark */
	(UBYTE *)player_name,	/* title */
	0,			      /* screen will be filled in later */
	0,			      /* bitmap */
	0,0,		      /* min width, height */
	-1,-1,		   /* max width, height */
	WBENCHSCREEN	/* type */
};

g_init(x,y)
{
	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);
	if (!IntuitionBase) {
		g_finish();
		printf("unable to open intuition\n");
		return(1);
	}

	GfxBase =  (struct GfxBase *)OpenLibrary("graphics.library",0);
	if (!GfxBase) {
		g_finish();
		printf("unable to open graphics library\n");
		return(1);
	}

	newwindow.LeftEdge = x;
	newwindow.TopEdge  = y+20;

	war_window = OpenWindow(&newwindow);
	if (!war_window) {
		g_finish();
		printf("unable to open window\n");
		return(1);
	}
	rp = war_window->RPort;
	SetBPen(rp, 0);
	SetAPen(rp, 1);
	SetDrMd(rp, JAM2);
	return(0);
}

g_finish()
{
	if (war_window)      CloseWindow(war_window);
	if (GfxBase)         CloseLibrary(GfxBase);
	if (IntuitionBase)   CloseLibrary(IntuitionBase);
}

g_show()
{
	char buf[40];
	int y = 8;
	int x = 8;

	Move(rp, x, y+=9);
	sprintf(buf, "%s player", colours[colour]);
	Text(rp, buf, strlen(buf));

	Move(rp, x, y+=9);
	sprintf(buf, "  Wins: %d", wins);
	Text(rp, buf, strlen(buf));

	Move(rp, x, y+=9);
	sprintf(buf, "Losses: %d", losses);
	Text(rp, buf, strlen(buf));

	Move(rp, x, y+=9);
	sprintf(buf, " Games: %d", wins+losses);
	Text(rp, buf, strlen(buf));
}
