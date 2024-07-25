// Struct for drawing game level and all game actors
typedef struct actors
{
    RECT ASnake[254];  // Anaconda cells array
    RECT BSnake[254];  // Bushmaster cells array
    RECT Grid[128];    // Max level size 65x65 cell
    RECT RApple;       // Apple px coord
    cpoint LevelWin;   // Level size in px
    int ALen;          // Anaconda len
    int BLen;          // Bushmaster len
    int GLen;          // Level grid array len
    int AColor[253];   // Anaconda sectors color array
    int BColor[253];   // Bushmaster sectors color array
    int AppleColor;    // Apple RGB color
} actors;