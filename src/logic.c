/* Almost all the logic of the game is here
   Warning! The game logic does not check the .win value during the first initialization.
   The '0' value must be assigned in main()
   Also main() should run initialization before the game loop */

#include <stdlib.h>
#include "snakestruct.h"

int IfPointArray(cpoint const *dot, snake *vyper)
{
     for (int i = vyper->len - 1; i >= 0; --i)
     {
          if (dot->x == vyper->body[i].x &&
              dot->y == vyper->body[i].y) return 1;
     }
     return 0;
}

fruit GetFruit(cpoint const *map, snake *vyper, snake * wutu)
{
    fruit new;
    do
    {
          new.coord.x = rand() % map->x;
          new.coord.y = rand() % map->y;
    }
    while (IfPointArray(&new.coord, vyper) ||
           IfPointArray(&new.coord, wutu));

    int FruitWeight = rand();  // temporary for .price

    if (FruitWeight % 7 == 0)
    {
          new.price = ColorGold;
          new.ttl = 2 * (map->x + map->y + (vyper->len + wutu->len));
    }
    else if (FruitWeight % 13 == 0)
    {
          new.price = ColorBlack;
          new.ttl = 2 * (map->x + map->y + (vyper->len + wutu->len));
    }
    else
    {
          new.price = ColorRed;
          new.ttl = 2 * (map->x + map->y + 2 * (vyper->len + wutu->len));
    }
    return new;
}

// Restarting the snakes and creating the apple
void SnakeRestart(savedata const *game, snake *vyper, snake *wutu, int *ticks, fruit *apple)
{
     *ticks = 256;             // Base snake speed: ~4 cell/s
     vyper->vectr.x = 0;       // The snake stands still
     vyper->vectr.y = 0;
     vyper->newvectr.x = 0;    // and is not going anywhere
     vyper->newvectr.y = 0;
     vyper->len = 1;
     vyper->body[0].x = game->mode ? (game->map.x / 3) : (game->map.x / 2);
     vyper->body[0].y = (game->map.y / 2);

     if ((vyper->coins > vyper->maxscore) && !game->mode)  // singleplayer maxscore
     {
          vyper->maxscore = vyper->coins;
     }
     vyper->coins = 100;

     wutu->vectr.x = 0;
     wutu->vectr.y = 0;
     wutu->newvectr.x = 0;
     wutu->newvectr.y = 0;
     wutu->len = 1;
     wutu->body[0].x = game->mode ? (game->map.x * 2/ 3) : -1;
     wutu->body[0].y = (game->map.y / 2);
     wutu->coins = 100;

     *apple = GetFruit(&game->map, vyper, wutu);  // Place the apple on the level
}

// Calculation direction of snake move
void SetVectr(cpoint *old, cpoint *new, int *len)
{
     if ((new->x != old->x *(-1)) || (new->y != old->y *(-1)) ||
         (*len == 1))
         {
               *old = *new;
         }
}

cpoint SetHeadS(cpoint neck, cpoint vect, cpoint map)
{
     cpoint newhead = {neck.x + vect.x, neck.y + vect.y};
     if (newhead.x == -1) newhead.x += map.x;
     if (newhead.x == map.x) newhead.x -= map.x;
     if (newhead.y == -1) newhead.y += map.y;
     if (newhead.y == map.y) newhead.y -= map.y;
     return newhead;
}

// The game logic
int SnakeLogic(savedata const *game, fruit *apple, int *ticks, snake *vyper, snake *wutu)
{
     SetVectr(&vyper->vectr, &vyper->newvectr, &vyper->len);
     if (!(vyper->vectr.x) && !(vyper->vectr.y))
          return -1;               // --> the snake stands still, skip

     cpoint head;
     if (!game->mode)
     {
          head.x = vyper->body[0].x + vyper->vectr.x;
          head.y = vyper->body[0].y + vyper->vectr.y;
          apple->ttl -= 4;

          if (((vyper->len != 1) && IfPointArray(&head, vyper)) ||
           head.x >= game->map.x || head.y >= game->map.y ||
           head.x < 0 || head.y < 0 || vyper->coins < 0)   // Don't bite yourself, and don't waste all coins else:
           return 0;               // --> restart round (Need run SnakeRestart() from main()!!! )
     }
     else
     {
          head = SetHeadS(vyper->body[0], vyper->vectr, game->map);
          apple->ttl -= 3;
          if ((vyper->len != 1) && IfPointArray(&head, vyper))
          {
               wutu->win++;
               return 0;           // --> restart round
          }
     }

     if ((head.x == apple->coord.x  && head.y == apple->coord.y))
     {
          apple->ttl = 0;
          vyper->coins = vyper->coins + 95 + 5*vyper->len * apple->price;

          if (game->mode)
          {
               vyper->len = (apple->price != ColorBlack) ? vyper->len + 1 :(vyper->len / 3) + 1;
                            *ticks = *ticks >=128 ? *ticks - 1 : *ticks;
          }
          else
          {
               vyper->len = (apple->price != ColorBlack) ? vyper->len + 1 : vyper->len;
                             *ticks = *ticks >=128 ? *ticks - 2 :
                             *ticks  >= 16 ? *ticks - 1 : *ticks;
          }
     }

     for (int i = vyper->len; i > 0; --i)
     {
          vyper->body[i] = vyper->body[i-1];
     }

     vyper->body[0] = head;   // insert head to body
     vyper->coins -= (vyper->len <= 64) ?  1 : 3;

     if (apple->ttl <= 0)
          *apple = GetFruit(&game->map, vyper, wutu);

     return 1;                // --> normal exit
}