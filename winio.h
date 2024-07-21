// Savefile structure and function prototypes

// Constants for possible game level sizes and application scaling
#define SMALLMAPX 24
#define SMALLMAPY 16
#define MEDIUMMAPX 30
#define MEDIUMMAPY 20
#define LARGEMAPX 36
#define LARGEMAPY 24
#define BIGSCALE 32
#define HUGESCALE 38
#define RENDERLAG 16  // Pause (ms) between game level redraw cycles

typedef struct savedata
{
     cpoint gamemap;  // Level size in cells
     int gamescale;   // Interface scale
     int lang;        // Interface language (only 0 or 1)
} savedata;

savedata ReadSavegame();
void DispatchVector(WPARAM key, cpoint * newvect1, cpoint * newvect2, DWORD * next_tick);
void DispatchMenu(WPARAM val, cpoint * map, int * scale, int * lang);
void WriteSavegame(cpoint maps, int scale, int lang);
void RunAppCopy(void);