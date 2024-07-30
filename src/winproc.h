// Struct for drawing game level and all game Actors
typedef struct Quad
{
    short left;
    short top;
    short right;
    short bottom;
} Quad;

typedef struct Actors
{
    Quad   ASnake[254];  // Anaconda cells array
    Quad   BSnake[254];  // Bushmaster cells array
    Quad   Grid[128];    // Max level size 65x65 cell
    int    AColor[254];  // Anaconda sectors color array
    int    BColor[254];  // Bushmaster sectors color array
    Quad   RApple;       // Apple px coord
    CPoint LevelWin;     // Level size in px
    int    AppleColor;   // Apple RGB color
    short  ALen;         // Anaconda len
    short  BLen;         // Bushmaster len
    short  GLen;         // Level grid array len
} Actors;