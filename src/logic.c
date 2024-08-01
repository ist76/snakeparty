/* Almost all the logic of the game is here
   Warning! The game logic does not check the .Win value during the first initialization.
   The '0' value must be assigned in main()
   Also main() should run initialization before the Game loop */

#include "snakestruct.h"

unsigned int LCGRand(unsigned int *RandState)  // Variant of Lehmer random generator
{
     unsigned long long Prod = (unsigned long long)*RandState * 279470273u;
     unsigned int x;
     Prod = (Prod & 0xffffffff) + 5 * (unsigned int)(Prod >> 32);
     Prod += 4;
     x = (unsigned int)Prod + 5 * (unsigned int)(Prod >> 32);
     return *RandState = x - 4;
}

static int IfPointArray(CPoint const *Dot, Snake *Vyper)
{
     for (short i = Vyper->Len - 1; i >= 0; --i)
     {
          if (Dot->x == Vyper->Body[i].x &&
              Dot->y == Vyper->Body[i].y) return 1;
     }
     return 0;
}

static Fruit GetFruit(CPoint const *Map, Snake *Vyper, Snake * Wutu, unsigned int *State)
{
    Fruit NewFruit;
    do
    {
          NewFruit.Coord.x = LCGRand(State) % Map->x;
          NewFruit.Coord.y = LCGRand(State) % Map->y;
    }
    while (IfPointArray(&NewFruit.Coord, Vyper) ||
           IfPointArray(&NewFruit.Coord, Wutu));

    unsigned int FruitWeight = LCGRand(State);  // temporary for .Price

    if (FruitWeight % 7 == 0)
    {
          NewFruit.Price = ColorGold;
          NewFruit.Ttl = 2 * (Map->x + Map->y + (Vyper->Len + Wutu->Len) / 2);
    }
    else if (FruitWeight % 13 == 0)
    {
          NewFruit.Price = ColorBlack;
          NewFruit.Ttl = 2 * (Map->x + Map->y + (Vyper->Len + Wutu->Len));
    }
    else
    {
          NewFruit.Price = ColorRed;
          NewFruit.Ttl = 3 * (Map->x + Map->y + (Vyper->Len + Wutu->Len) / 4);
    }
    return NewFruit;
}

// Restarting the snakes and creating the Apple
void SnakeRestart(SaveData const *Game, Fruit *Apple, int *Ticks, Snake *Vyper, Snake *Wutu, unsigned int *State)
{
     *Ticks = DEFTICKS;        // Base Snake speed: ~4 cell/s if 256
     Vyper->Vectr.x    = 0;    // The Snake stands still
     Vyper->Vectr.y    = 0;
     Vyper->NewVectr.x = 0;    // and is not going anywhere
     Vyper->NewVectr.y = 0;
     Vyper->Len        = 1;
     Vyper->Body[0].x  = Game->Mode ? (Game->Map.x / 3) : (Game->Map.x / 2);
     Vyper->Body[0].y  = (Game->Map.y / 2);

     if ((Vyper->Coins > Vyper->MaxScore) && !Game->Mode)  // singleplayer MaxScore
     {
          Vyper->MaxScore = Vyper->Coins;
     }
     Vyper->Coins = DEFCOINS;

     Wutu->Vectr.x    = 0;
     Wutu->Vectr.y    = 0;
     Wutu->NewVectr.x = 0;
     Wutu->NewVectr.y = 0;
     Wutu->Len        = 1;
     Wutu->Body[0].x  = Game->Mode ? (Game->Map.x * 2/ 3) : -1;
     Wutu->Body[0].y  = (Game->Map.y / 2);
     Wutu->Coins      = DEFCOINS;

     *Apple = GetFruit(&Game->Map, Vyper, Wutu, State);  // Place the Apple on the level
}

// Calculation direction of Snake move
static inline void SetVectr(CPoint *old, CPoint *NewFruit, short *Len)
{
     if ((NewFruit->x != old->x *(-1)) || (NewFruit->y != old->y *(-1)) ||
         (*Len == 1))
         {
               *old = *NewFruit;
         }
}

static inline CPoint SetHeadS(CPoint neck, CPoint vect, CPoint Map)
{
     CPoint newhead = {neck.x + vect.x, neck.y + vect.y};
     if (newhead.x == -1)     newhead.x += Map.x;
     if (newhead.x == Map.x)  newhead.x -= Map.x;
     if (newhead.y == -1)     newhead.y += Map.y;
     if (newhead.y == Map.y)  newhead.y -= Map.y;
     return newhead;
}

static inline void Crawl(Snake *Vyper, CPoint *Head)
{
     for (short i = Vyper->Len; i > 0; --i)
     {
          Vyper->Body[i] = Vyper->Body[i-1];
     }

     Vyper->Body[0].x = Head->x;   // insert Head to Body
     Vyper->Body[0].y = Head->y;
     Vyper->Coins  -= (Vyper->Len <= 64) ?  1 : 3;
}

// The Game logic
int SnakeLogic(SaveData const *Game, Fruit *Apple, int *Ticks, Snake *Vyper, Snake *Wutu, unsigned int *State)
{
     SetVectr(&Vyper->Vectr, &Vyper->NewVectr, &Vyper->Len);
     if (!(Vyper->Vectr.x) && !(Vyper->Vectr.y))  return -1;  // --> the Snake stands still, skip

     CPoint Head;
     if (!Game->Mode)
     {
          Head.x = Vyper->Body[0].x + Vyper->Vectr.x;
          Head.y = Vyper->Body[0].y + Vyper->Vectr.y;
          Apple->Ttl -= 5;

          if (((Vyper->Len != 1) && IfPointArray(&Head, Vyper)) ||
                Head.x >= Game->Map.x || Head.y >= Game->Map.y ||
                Head.x < 0 || Head.y < 0 || Vyper->Coins <= 0)
          return 0;            // --> restart round (Need run SnakeRestart() from main()!!! )
     }
     else
     {
          Head = SetHeadS(Vyper->Body[0], Vyper->Vectr, Game->Map);
          Apple->Ttl -= 3;
          if (((Vyper->Len != 1) && IfPointArray(&Head, Vyper)) || Vyper->Coins <= 0)
          {
               Wutu->Win++;
               return 0;       // --> restart round
          }
     }

     if ((Head.x == Apple->Coord.x  && Head.y == Apple->Coord.y))
     {
          Apple->Ttl = 0;
          Vyper->Coins = Vyper->Coins + 95 + 5*Vyper->Len * Apple->Price;

          if (Game->Mode)
          {
               Vyper->Len = (Apple->Price != ColorBlack) ? Vyper->Len + 1 :(Vyper->Len / 3) + 1;
                             *Ticks = *Ticks >=128 ? *Ticks - 1 : *Ticks;
          }
          else
          {
               Vyper->Len = (Apple->Price != ColorBlack) ? Vyper->Len + 1 : Vyper->Len;
                             *Ticks = *Ticks >=160 ? *Ticks - 2 :
                             *Ticks  >= 16 ? *Ticks - 1 : *Ticks;
          }
     }
     Crawl(Vyper, &Head);
     if (Apple->Ttl <= 0)
        *Apple = GetFruit(&Game->Map, Vyper, Wutu, State);
     return 1;                // --> normal exit
}