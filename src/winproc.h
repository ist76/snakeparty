// Struct for drawing game level and all game actors
typedef struct quad
{
    short left;
    short top;
    short right;
    short bottom;
} quad;

typedef struct actors
{
    quad   ASnake[254];  // Anaconda cells array
    quad   BSnake[254];  // Bushmaster cells array
    quad   Grid[128];    // Max level size 65x65 cell
    int    AColor[254];  // Anaconda sectors color array
    int    BColor[254];  // Bushmaster sectors color array
    quad   RApple;       // Apple px coord
    cpoint LevelWin;     // Level size in px
    int    AppleColor;   // Apple RGB color
    short  ALen;         // Anaconda len
    short  BLen;         // Bushmaster len
    short  GLen;         // Level grid array len
} actors;