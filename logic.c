// Almost all the logic of the game is here

#include <stdio.h>
#include <stdlib.h>
#include "snakestruct.h"

// Warning! The game logic does not check the maxscore value during the first initialization.
// The '0' value must be assigned in main
// Also main should run initialization before the game loop
void SnakeRestart(cpoint const *gamemap, snake *vyper, snake *wutu, int *ticks)
{
     *ticks = 256;
     vyper->vectr.x = 0;
     vyper->vectr.y = 0;
     vyper->newvectr.x = 0;
     vyper->newvectr.y = 0;
     vyper->len = 1;
     vyper->body[0].x = (gamemap->x / 3);
     vyper->body[0].y = (gamemap->y / 2);
     vyper->coins = 100;
     wutu->vectr.x = 0;
     wutu->vectr.y = 0;
     wutu->newvectr.x = 0;
     wutu->newvectr.y = 0;
     wutu->len = 1;
     wutu->body[0].x = (gamemap->x * 2/ 3);
     wutu->body[0].y = (gamemap-> y / 2);
     wutu->coins = 100;
}

int IfCannibal(cpoint const *dot, int const *len, cpoint const *body)
{
     for (int i = *len-1; i >= 0; --i)
     {
          if ((dot->x == body[i].x) && (dot->y == body[i].y)) return 1;
     }
     return 0;
}

// Calculation direction of snake move
void SetVectr(cpoint *old, cpoint *new, int *len)
{
     if ((new->x != old->x *(-1)) || (new->y != old->y *(-1)) ||
         (*len == 1)) *old = *new;
}

// Calculating head coordinates
cpoint SetHead (cpoint neck, cpoint vect, cpoint border)
{
     cpoint newhead = {neck.x + vect.x, neck.y + vect.y};
     if (newhead.x == -1) newhead.x += border.x;
     if (newhead.x == border.x) newhead.x -= border.x;
     if (newhead.y == -1) newhead.y += border.y;
     if (newhead.y == border.y) newhead.y -= border.y;
     return newhead;
}

// Must be run for the first time before the game loop
cpoint GetApple(cpoint const *gamemap, int const *len1, cpoint const *body1, int const *len2, cpoint const *body2)
{
    cpoint new;
    do
    {
          new.x = rand() % gamemap->x;
          new.y = rand() % gamemap->y;
    }
    while ((IfCannibal(&new, len1, body1) == 1) && (IfCannibal(&new, len2, body2) == 1));
    return new;
}

// The game loop checks for the return value. If "0" is received, SnakeRestart is called
int SnakeLogic(cpoint const *gamemap, cpoint *apple, int *ticks, snake *vyper, snake * wutu)
{
     SetVectr(&vyper->vectr, &vyper->newvectr, &vyper->len);
     if ((vyper->vectr.x == 0) && (vyper->vectr.y == 0)) return 0;  // --> the snake stands still, skip
     cpoint head = SetHead(vyper->body[0], vyper->vectr, *gamemap);
     
     if (((vyper->len != 1) && IfCannibal(&head, &vyper->len, vyper->body)) ||
        (vyper->coins < 0))  // Don't bite yourself, and don't waste all coins else:
        {
             wutu->win++;
             return 1;            // --> restart round
        }
     
     if ((IfCannibal(apple, &vyper->len, vyper->body)) || ((head.x == apple->x) && (head.y == apple->y))) // not good (((
     {
          vyper->coins = (vyper->len%10 == 0)?                 // Score growth depending on tail length
                         (vyper->coins + 50*vyper->len):       // If the length is a multiple of 10
                         (vyper->coins + 95 + 5*vyper->len);   // Else

          ++vyper->len; // The snake has become longer, and the coordinate of the tail has already moved to where we need it
          *ticks = *ticks >=128 ? *ticks -1 : *ticks;  // Speed ​​up the game with every apple you eat
          *apple = GetApple(gamemap, &vyper->len, vyper->body, &wutu->len, wutu->body);
     }
     
     for (int i = vyper->len; i > 0; --i) vyper->body[i] = vyper->body[i-1];
     vyper->body[0] = head; // insert head to body
     vyper->coins -= (vyper->len <= 64) ?  1 : 2;
     return 0; // --> normal exit
}