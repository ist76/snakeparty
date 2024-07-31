/* Declarations for main() */

unsigned int RandState;  // Global state for custom random numbers generator

int  SnakeLogic(SaveData const *Game, Fruit *Apple, int *Ticks, Snake *Vyper, Snake *Wutu);
void SnakeRestart(SaveData const *Game, Snake *Vyper, Snake *Wutu, int *Ticks, Fruit *Apple);
void ActorsShow(HDC dc, Actors *AllObj, unsigned char Mode);
void SolutionShow(HDC dc, HFONT Font, RECT * const rt, wchar_t *Message);

static inline void MakeMenu(HWND hwnd, SaveData *GSets, GameLang *Marks);
static inline void GetSnakesCells(Actors *Allobj, Snake const *Vyper, Snake const *Wutu, SaveData *const Sets);
static inline void RunAppCopy(void);
static inline void SetApple(Actors *Allobj, Fruit *Apple, short Scale);
static inline void SetInfo(wchar_t *Score, GameLang *Marks, Snake *Vyper, SaveData *Sets);
static void        GetSnakeColors(Actors* Allobj, unsigned char Mode);
static void        GetGrid(Actors *Allobj, CPoint Map, short Scale);
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
// static void WriteGameLang(void)