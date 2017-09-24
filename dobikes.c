
/*
 * this code is for moving the cycles around on the screen and
 * updating their status at intervals
 */

#include "cycles.h"

extern int xdir[], ydir[];
extern int num_players;
extern int num_waiting;
extern int num_alive;

int ties;

move_cycles(list)
LIST *list;
{
	register CYCLE *c, *c2;
	int na;

	na = num_alive;

	c = (CYCLE *)list->lh_Head;
	if (c->node.ln_Succ) {		/* rotate the list 		   */
		Remove(c);		/* so that each cycles gets a turn */
		AddTail(list,c);	/* at first reply 		   */
	}
	c = (CYCLE *)list->lh_Head;

	while (c2 = (CYCLE *)c->node.ln_Succ) {
		switch (c->status) {

		case STATUS_GOT_MOVE:
		case STATUS_NEED_NEW:
			link_pix(c->colour,c->x,c->y,c->dir);
			c->x += xdir[c->dir];
			c->y += ydir[c->dir];
			if (rpix(c->x,c->y)) {
				c->status = STATUS_LOSER;
				num_alive--;
				if (num_alive == 0) {
					ties++;
					draw_scores();
					flash_colour(0);
				}
			}
			else {
				if (na>1)
					c->status = STATUS_NEED_NEW;
				else {
					scores[c->colour]++;
					draw_scores();
					c->status = STATUS_WINNER;
					flash_colour(c->colour+3);
					/* avoid title colours */
					num_alive--;
				}
				wpix(c->colour,c->x,c->y,c->dir);
			}

			if (c->msg) {
				c->msg->x      = c->x;
				c->msg->y      = c->y;
				c->msg->dir    = c->dir;
				c->msg->status = c->status;
				ReplyMsg(c->msg);
				c->msg = NULL;
			}
			break;

		default:
			break;
		}
		c = c2;
	}
}


free_cycles(list)
LIST *list;
{
	register CYCLE *c,*c2;

	c = (CYCLE *)list->lh_Head;

	/* check all cycles... */

	while (c2 = (CYCLE *)c->node.ln_Succ) {

		/* if there is any sort of message pending */

		if (c->msg) {
			/* inform the client that he has been terminated */
			c->msg->status = STATUS_REMOVED;
			ReplyMsg(c->msg);
			c->msg = NULL;
			num_players--;
		}

		/* clean up client... */

		Remove(c);
		FreeMem(c,sizeof(CYCLE));
		c = c2;
	}
}

restart_cycles(list)
LIST *list;
{
	register CYCLE *c,*c2;

	c = (CYCLE *)list->lh_Head;

	while (c2 = (CYCLE *)c->node.ln_Succ) {
		if (c->status == STATUS_AWAITING) {
			c->x	       = rand()%(X_SIZE-10)+5;
			c->y	       = rand()%(Y_SIZE-10)+5;
			c->dir	       = DIR_RIGHT;
			c->status      = STATUS_NEED_NEW;
			c->msg->x      = c->x;
			c->msg->y      = c->y;
			c->msg->status = c->status;
			c->msg->dir    = c->dir;
			wpix(c->colour,c->x,c->y,c->dir);

			ReplyMsg(c->msg);
			c->msg = NULL;
		}
		c = c2;
	}
	num_waiting = 0;
	num_alive = num_players;
}
