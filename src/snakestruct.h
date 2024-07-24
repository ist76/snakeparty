// Basic structures - cell point, snake and fruit

typedef struct cpoint     // The logic of the game is calculated in "cells"
{
     int x, y;
} cpoint;

typedef struct snake      // All snake data
{
     int coins;           // Current score
     int maxscore;        // Game bestscore
     cpoint vectr;        // Current direction of the snake
     cpoint newvectr;     // Player-entered direction
     int len;             // Current snake length (in segments)
     int win;             // Wins counter
     cpoint body[254];    // An array of segment coordinates (I hope 254 is enough)
} snake;

enum FruitColor
{
     ColorBlack = -10,
     ColorRed = 1,
     ColorGold = 5,
};

typedef struct fruit
{
     cpoint coord;
     int price;
     int ttl;
} fruit;

typedef struct savedata
{
     cpoint map;  // Level size in cells
     int scale;   // Interface scale
     int mode;    // 0 - singleplayer 1 - two players
     int lang;    // Interface language (only 0 or 1)
     int maxs;    // Global game best score
} savedata;