// Basic structures - cell point, snake and fruit

typedef struct cpoint    // The logic of the game is calculated in "cells"
{
     int x, y;
} cpoint;

typedef struct snake     // All snake data
{
     cpoint body[254];   // An array of segment coordinates (I hope 254 is enough)
     cpoint vectr;       // Current direction of the snake
     cpoint newvectr;    // Player-entered direction
     int coins;          // Current score
     int maxscore;       // Game bestscore
     size_t len;         // Current snake length (in segments)
     size_t win;         // Wins counter
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
     cpoint map;           // Level size in cells
     int maxs;             // Global game best score
     size_t scale;         // Interface scale
     unsigned char mode;   // 0 - singleplayer 1 - two players
     unsigned char lang;   // Interface language (only 0 or 1)
} savedata;