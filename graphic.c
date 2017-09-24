/*
 * here we have all of the graphics rendering code...
 *
 */
#include <intuition/intuition.h>
#include "cycles.h"

#define INTUITION_REV 29
#define GRAPHICS_REV 29
#define X_BYTES ((X_SIZE/8)+!!(X_SIZE%8))

struct NewScreen newscreen = {
	0,0,			/* left edge, top edge */
	320,200,		/* width, height */
	3,			/* depth */
	0,1,			/* DetailPen, BlockPen */
	0,			/* viewmodes */
	CUSTOMSCREEN,		/* type */
	NULL,			/* font */
	(UBYTE *)"  Cycles",	/* title */
	NULL,			/* Gadgets */
	NULL			/* CustomBitMap*/
};

struct NewWindow newwindow = {
	0,0,			/* left edge, top edge */
	15,10,			/* width, height */
	0,1,			/* detail pen, block pen */
	CLOSEWINDOW,		/* IDCMP Flags*/
	WINDOWCLOSE,		/* flags */
	0,			/* first gadget */
	0,			/* checkmark */
	0,			/* title */
	0,			/* screen will be filled in later */
	0,			/* bitmap */
	0,0,			/* min width, height */
	-1,-1,			/* max width, height */
	CUSTOMSCREEN		/* type */
};

UWORD scr_colours[] = {
	0x000, 0xf80, 0x002, 0xf00,
	0x00f, 0xfff, 0x0f0, 0xff0
};

BYTE scr_image[ X_BYTES * Y_SIZE];

struct IntuitionBase	*IntuitionBase;
struct GfxBase		*GfxBase;
struct Screen		*war_screen;
struct Window		*war_window;
struct RastPort		*rp;
struct Screen		*OpenScreen();
struct Window		*OpenWindow();
APTR              OpenLibrary();

g_init()
{
	IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",INTUITION_REV);
	if (!IntuitionBase) {
		g_finish();
		printf("unable to open intuition\n");
		return(1);
	}

	GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",GRAPHICS_REV);
	if (!GfxBase) {
		g_finish();
		printf("unable to open graphics library\n");
		return(1);
	}

	war_screen = OpenScreen(&newscreen);
	if (!war_screen) {
		g_finish();
		printf("unable to open screen\n");
		return(1);
	}

	LoadRGB4(&war_screen->ViewPort, scr_colours, 8);

	rp = &war_screen->RastPort;
	newwindow.Screen = war_screen;

	war_window = OpenWindow(&newwindow);
	if (!war_window) {
		g_finish();
		printf("unable to open window\n");
		return(1);
	}
	return(0);
}

g_restart()
{
	int i;
   
	for (i = 0; i < X_BYTES*Y_SIZE; i++) 
      scr_image[i] = 0;

	SetAPen(rp,0);
	RectFill(rp,0,10,319,199);
	SetAPen(rp,1);
}

g_finish()
{
	if (war_window)      CloseWindow(war_window);
	if (war_screen)      CloseScreen(war_screen);
	if (GfxBase)         CloseLibrary(GfxBase);
	if (IntuitionBase)   CloseLibrary(IntuitionBase);
}

static int power[] = {1,2,4,8,16,32,64,128};

rpix(x,y)
{
	if (x<0 || y<0 || x>=X_SIZE || y>=Y_SIZE)
		return(1);

	return(scr_image[(x>>3) + y*X_BYTES] & power[x&7]);
}

typedef struct {
	int x,y;
} PAIR;

PAIR old_data[] = { {4,7},{0,4},{4,0},{7,4} };
PAIR new_data[] = { {4,0},{7,4},{4,7},{0,4} };

void link_pix(c,x,y,dir)
{
	int sx, sy;

	if (x<0 || y<0 || x>=X_SIZE || y>=Y_SIZE)
		return;

	SetAPen(rp,c+3);	/* don't use title bar colours */
	sx = x<<3;
	sy = (y<<3)+10;
	Move(rp,sx+4,sy+4);
	Draw(rp,sx + new_data[dir].x, sy+new_data[dir].y);
}

void wpix(c,x,y,dir)
{
	int sx, sy;

	if (x<0 || y<0 || x>=X_SIZE || y>=Y_SIZE)
		return;

	scr_image[(x>>3) + y*X_BYTES] |= power[x&7];

	SetAPen(rp,c+3);	/* don't use title bar colours */
	sx = x<<3;
	sy = (y<<3)+10;
	Move(rp,sx + old_data[dir].x, sy+old_data[dir].y);
	Draw(rp,sx+4,sy+4);
}

flash_colour(c)
{
	UWORD rgb;
	int i,red,grn,blu;
	struct ViewPort *vp;

	vp  = &war_screen->ViewPort;
	rgb = GetRGB4(vp->ColorMap,c);

	red = (rgb >> 8) & 0x0f;
	grn = (rgb >> 4) & 0x0f;
	blu =  rgb       & 0x0f;

	for (i=0;i<10;i++) {
		Delay(5);
		WaitTOF();
		SetRGB4(vp,c,8^red,8^grn,8^blu);
		Delay(5);
		WaitTOF();
		SetRGB4(vp,c,red,grn,blu);
	}
}

draw_scores()
{
	int i;
	char buf[20];
	extern int ties;

	SetBPen(rp,1);
	SetDrMd(rp,JAM2);
	Move(rp,90,7);
	SetAPen(rp,0);
	sprintf(buf,"%-3d ",ties);
	Text(rp,buf,strlen(buf));
	for (i=0;i<MAX_COLOURS;i++) {
		SetAPen(rp,i+3);
		sprintf(buf,"%-3d ",scores[i]);
		Text(rp,buf,strlen(buf));
	}
}
