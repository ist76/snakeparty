/* Basic structures - cell point, Snake and Fruit */

#define DEFCOINS 100        // Default game score on start
#define DEFTICKS 256        // Default time of game logic refresh (~4/s)

typedef struct CPoint       // The logic of the game is calculated in "cells"
{
     short x, y;
} CPoint;

typedef struct Snake        // All Snake data
{
     CPoint Body[254];      // An array of segment coordinates (I hope 254 is enough)
     CPoint Vectr;          // Current direction of the Snake
     CPoint NewVectr;       // Player-entered direction
     int    Coins;          // Current score
     int    MaxScore;       // Game bestscore
     short  Len;            // Current Snake length (in segments)
     short  Win;            // Wins counter
} Snake;

enum FruitColor             // Reward (or penalty) for taking apple
{
     ColorBlack = -10,
     ColorRed   = 1,
     ColorGold  = 5,
};

typedef struct Fruit        // Data of apple:
{
     CPoint Coord;          // .x and .y coordinates
     int    Price;          // Reward (or penalty) for taking (set in enum FruitColor)
     int    Ttl;            // Lifetime, upon expiration a new apple is generated
} Fruit;

typedef struct SaveData     // Stored application data
{
     CPoint        Map;     // Level size in cells
     int           MaxS;    // Global game best score
     short         Scale;   // Interface Scale
     unsigned char Mode;    // 0 - singleplayer 1 - two players
     unsigned char Lang;    // Interface language (only 0 or 1)
} SaveData;