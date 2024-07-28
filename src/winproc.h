// Struct for drawing game level and all game actors
typedef struct actors
{
    RECT ASnake[254];    // Anaconda cells array
    RECT BSnake[254];    // Bushmaster cells array
    RECT Grid[128];      // Max level size 65x65 cell
    size_t AColor[253];  // Anaconda sectors color array
    size_t BColor[253];  // Bushmaster sectors color array
    RECT RApple;         // Apple px coord
    cpoint LevelWin;     // Level size in px
    size_t ALen;         // Anaconda len
    size_t BLen;         // Bushmaster len
    size_t GLen;         // Level grid array len
    size_t AppleColor;   // Apple RGB color
} actors;