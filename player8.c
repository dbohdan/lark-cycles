#include "cycles.h"
#include <exec/types.h>
#include <exec/devices.h>
#include <devices/gameport.h>
#include <devices/inputevent.h>

char player_name[] = "Joystick";

static struct MsgPort  *gameport;
static struct IOStdReq *gameio;
static BYTE gamebuffer[sizeof(struct InputEvent)];
static BYTE *gamebuff;
static struct InputEvent *gamedata;

struct IOStdReq *CreateStdIO();

strategy_init()
{

	gameport = CreatePort(0, 0);
	if (!gameport) {
		printf("Error, couldn't create port\n");
		return(-1);
	}

	gameio = CreateStdIO(gameport);
	if (!gameio) {
		printf("Error, couldn't create StdIO message\n");
		DeletePort(gameport);
		return(-1);
	}

	if (OpenDevice("gameport.device", 1, gameio, 0)) {
		printf("Error, couldn't open gameport.device\n");
		DeleteStdIO(gameio);
		DeletePort(gameport);
		return(-1);
	}

	if (SetCType(GPCT_ABSJOYSTICK)) {
		printf("Error, can't set controller type\n");
		CloseDevice(gameio);
		DeleteStdIO(gameio);
		DeletePort(gameport);
		return(-1);
	}

	if (SetTrigger(GPTF_UPKEYS | GPTF_DOWNKEYS, 50)) {
		printf("Error, can't set trigger type\n");
		CloseDevice(gameio);
		DeleteStdIO(gameio);
		DeletePort(gameport);
		return(-1);
	}

	gameio->io_Command = GPD_READEVENT;
	gameio->io_Data    = (APTR)gamebuffer;
	gameio->io_Length  = sizeof(struct InputEvent);
	gameio->io_Flags   = 0;

	gamedata = (struct InputEvent *)gamebuffer;
}

strategy_finish()
{
	CloseDevice(gameio);
	DeleteStdIO(gameio);
	DeletePort(gameport);
}

strategy()
{
	int code;
	int xmove, ymove;
	int x, y, dir;

	GetInfo(&x, &y, &dir);

	gameio->io_Length  = sizeof(struct InputEvent);
	SendIO(gameio);
	WaitPort(gameport);
	while (GetMsg(gameport)) ;

	code  = gamedata->ie_Code;
	xmove = gamedata->ie_X;
	ymove = gamedata->ie_Y;

	if (ymove)
		return( (ymove<0) ? DIR_UP : DIR_DOWN );

	if (xmove)
		return( (xmove<0) ? DIR_LEFT : DIR_RIGHT );

	return(dir);
}

static SetCType(type)
{
	gameio->io_Command = GPD_SETCTYPE;
	gameio->io_Length  = 1;
	gameio->io_Data    = (APTR)gamebuff;
	*gamebuff	       = type;

	SendIO(gameio);
	WaitPort(gameport);
	GetMsg(gameport);
	return(gameio->io_Error);
}

static SetTrigger(keys,timeout)
{
	struct GamePortTrigger gpt;

	gameio->io_Command = GPD_SETTRIGGER;
	gameio->io_Length  = sizeof(gpt);
	gameio->io_Data    = (APTR)&gpt;

	gpt.gpt_Keys	   = keys;
	gpt.gpt_Timeout	= timeout;
	gpt.gpt_XDelta	   = 1;
	gpt.gpt_YDelta	   = 1;

	return(DoIO(gameio));
}
