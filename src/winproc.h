// Struct for drawing game level and all game actors
typedef struct actors
{
    RECT ASnake[254];  // Anaconda cells array
    RECT BSnake[254];  // Bushmaster cells array
    RECT Grid[128];    // Max level size 65x65 cell
    RECT RApple;       // Apple px coord
    cpoint LewelWin;   // Level size in px
    int alen;          // Anaconda len
    int blen;          // Bushmaster len
    int glen;          // Level grid array len
    int AppleColor;    // Apple RGB color
} actors;