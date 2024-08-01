/* Interface strings structure and some function prototypes */

// Constants for possible game level sizes and application scaling
#define SMALLMAPX  24
#define SMALLMAPY  16
#define MEDIUMMAPX 30
#define MEDIUMMAPY 20
#define LARGEMAPX  36
#define LARGEMAPY  24
#define BIGSCALE   32
#define HUGESCALE  40
#define RENDERLAG  16       // Pause (ms) between game level redraw cycles

typedef struct GameLang
{
     wchar_t str1503[192];  // Scoreboard Message3
     wchar_t str1501[40];   // Scoreboard Message1
     wchar_t str1502[40];   // Scoreboard Message2
     wchar_t str1008[16];   // Map size
     wchar_t str1009[16];   // Scale
     wchar_t str1110[16];   // Game Mode
     wchar_t str1100[16];   // Lang
     wchar_t str1001[16];   // Small
     wchar_t str1002[16];   // Medium
     wchar_t str1003[16];   // Large
     wchar_t str1011[16];   // Big
     wchar_t str1012[16];   // Huge
     wchar_t str1013[16];   // Single Player
     wchar_t str1014[16];   // Coop
} GameLang;

SaveData ReadSavegame(void);
GameLang ReadGamelang(unsigned char Num);
void     DispatchVector(WPARAM Key, CPoint *NewVect1, CPoint *NewVect2, DWORD *NextTick, unsigned char Mode);
void     DispatchMenu(WPARAM Val, SaveData *GameSettings);
void     WriteSavegame(SaveData const *GameSettings, int MaxScore);