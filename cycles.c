/*
 * main cycles program ... wait for messages to come in and 
 * do "the right thing"
 *
 */

#include <intuition/intuition.h>
#include <exec/memory.h>
#include <devices/timer.h>
#include <libraries/dos.h>
#include "cycles.h"

extern BYTE scr_image[];

int xdir[] = {  0,  1,  0, -1 };    /* up, right, down, left */
int ydir[] = { -1,  0,  1,  0 };    /* up, right, down, left */
int TIME    = 6;
int PLAYERS = 2;
int GAMES   = 0;
int scores[MAX_COLOURS];

IMAGE   scr_data;       /* screen image data */
LIST    cycle_list;     /* list of current cycles */
TIMEREQ *tr;            /* timer ExtIO request structure */
PORT    *sPort;         /* server port */
PORT    *tPort;         /* timer port */
int num_players = 0;    /* how many players are there */
int num_waiting = 0;    /* how many are waiting for new game */
int num_alive = 0;      /* number of cycles that are still moving */

extern struct Window *war_window;   /* our window c/w close gadget */

main(argc,argv)
int argc;
char **argv;
{
    ORDER   *msg;           /* order from client */
    CYCLE   *cycle;         /* current cycle structure */
    int portsig, closesig;  /* signal bits for our ports */
    int signals;            /* received signals */
    int used_colours = 0;   /* bitmask showing which colours are used */
    int i;                  /* misc */
    int games_played;

    /* initialize screen data structures, will tell clients the shape
     * of the screen by providing a pointer to this data
     */
    scr_data.x   = X_SIZE;
    scr_data.y   = Y_SIZE;
    scr_data.buf = scr_image;
    tr = 0;
    games_played = 0;

    for (i=1; i < argc; i++) {
        switch (argv[i][0]) {
            case 't':
                TIME = atoi(argv[i]+1);
                break;
            case 'p':
                PLAYERS = atoi(argv[i]+1);
                if (PLAYERS < 2) PLAYERS = 2;
                break;
            case 'g':
                GAMES = atoi(argv[i]+1);
                break;
        }
    }
    sPort = CreatePort("Cycle Server",0L);

    if (!sPort) {
        printf("Error, can't open server port\n");
        terminate();
    }

    tr = Timer_Open(UNIT_MICROHZ,sPort);
    if (!tr) terminate();

    /* open up custom screen etc... */
    if (g_init()) terminate();

    draw_scores();

    /* do setup */
    g_restart();

    portsig = (1 << (sPort->mp_SigBit));
    closesig = (1 << war_window->UserPort->mp_SigBit);

    srand(time(0));  /* seed the random number generator */

    NewList(&cycle_list);

    Timer_Post(tr,TIME/1000,(TIME%1000)*1000);

    for (;;) {
        /* wait for either a time event or a client */
        signals = Wait(closesig | portsig | SIGBREAKF_CTRL_C);

        if (signals & portsig) {
            while (msg = (ORDER *)GetMsg(sPort)) {

                if ((TIMEREQ *)msg == tr) { /* time request reply */
                    if (num_alive) {
                        move_cycles(&cycle_list);
                        Timer_Post(tr,
                                TIME*num_alive/1000,           /* seconds */
                                ((num_alive*TIME)%1000)*1000   /* micros  */
                                );
                    }
                    else
                        Timer_Post(tr,TIME/1000,(TIME%1000)*1000);
                        
                    continue;
                }
                cycle  = msg->cycle;
    
                if (!cycle) {
                    for (i = 0; i < 8; i++)
                        if ( (~used_colours) & (1 << i) ) break;
            
                    if (i == MAX_COLOURS) {
                        printf("Error, no colours left for the new cycle\n");
                        msg->status = STATUS_REMOVED;
                        ReplyMsg(msg);
                        continue;
                    }
                    cycle = (CYCLE *)AllocMem(sizeof(CYCLE),MEMF_PUBLIC);
                    if (!cycle) {
                        printf("Error, can't get memory for new cycle\n");
                        msg->status = STATUS_REMOVED;
                        ReplyMsg(msg);
                        continue;
                    }
                    AddHead(&cycle_list, &cycle->node);
                    num_players++;
                    msg->cycle    = cycle;
                    msg->table    = &scr_data;
                    cycle->status = STATUS_LOSER;
                    cycle->dir    = DIR_RIGHT;
                    cycle->x      = rand() % (X_SIZE-10)+5;
                    cycle->y      = rand() % (Y_SIZE-10)+5;
                    cycle->colour = i;
                    cycle->msg    = NULL;
                    used_colours |= (1 << i);
                    scores[i]     = 0;
                }
                switch (msg->order) {
        
                    case ORD_DIRECTION:
                        if (cycle->status < STATUS_DEAD ) {
                            cycle->dir    = msg->dir;
                            cycle->status = STATUS_GOT_MOVE;
                            cycle->msg    = msg;
                        }
                        else {
                            msg->status   = cycle->status;
                            ReplyMsg(msg);
                        }
                        continue;
                    case ORD_AWAIT:
                        if (cycle->status < STATUS_DEAD)  /* still alive */
                            num_alive--;
            
                        cycle->msg    = msg;
                        cycle->status = STATUS_AWAITING;
                        num_waiting++;
                        if (num_waiting == num_players) {
                            if (GAMES >0 && games_played >= GAMES)
                                terminate();
                            else if (num_players >= PLAYERS) {
                                g_restart();
                                restart_cycles(&cycle_list);
                                games_played++;
                            }
                        }
                        continue;
            
                    case ORD_RETIRE:
                        Remove(cycle);
                        if (cycle->status < STATUS_DEAD)  /* still alive */
                            num_alive--;
                        FreeMem(cycle,sizeof(CYCLE));
                        num_players--;
                        used_colours &= ~(1<<cycle->colour);
                        msg->status   = STATUS_REMOVED;
                        ReplyMsg(msg);
                        continue;
            
                    default:
                        printf("Got a invalid request.  Ignoring\n");
                        ReplyMsg(msg);
                        continue;
                }
            }
        }
        if (signals & SIGBREAKF_CTRL_C || signals & closesig)
            terminate();
    }
}


terminate()
{
    ORDER *msg;

    free_cycles(&cycle_list);

    if (tr) {
        WaitIO(tr);
        tr->tr_node.io_Message.mn_ReplyPort=0L;
        Timer_Close(tr);
    }

    /* inform all players that are still active that we are shutting down */
    while (num_players) {
        WaitPort(sPort);
        while (msg = (ORDER *)GetMsg(sPort)) {
            msg->status = STATUS_REMOVED;
            /* if a new player tries do join, don't decrement the count */
            /* the actual structure has already been free so don't look */
            /* at it */
            if (msg->cycle) num_players--;
                ReplyMsg(msg);
        }
    }
    g_finish();

    /* here we clear out the Port before deleting it, it is important
     * to Forbid() first so that no new messages arrive at the port
     * between the time we last look at it and the time it is DeletePort'ed
     */

    Forbid();
    while (msg = (ORDER *)GetMsg(sPort)) {
       msg->status = STATUS_REMOVED;
       ReplyMsg(msg);
    }
    DeletePort(sPort);
    Permit();
    exit(0);
}
