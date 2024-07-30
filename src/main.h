// Declarations for main()

int  SnakeLogic(SaveData const *Game, Fruit *Apple, int *Ticks, Snake *Vyper, Snake *Wutu);
void SnakeRestart(SaveData const *Game, Snake *Vyper, Snake *Wutu, int *Ticks, Fruit *Apple);
void ActorsShow(HDC dc, Actors *AllObj, unsigned char Mode);
void ScoresShow(HDC dc, Snake *Vyper, HFONT Font, RECT * const rt, wchar_t *Message, unsigned char Mode);
void SolutionShow(HDC dc, HFONT Font, RECT * const rt, wchar_t *Message);

static inline void MakeMenu(HWND hwnd, SaveData *GSets, GameLang *Marks);
static inline void GetSnakesCells(Actors *Allobj, Snake const *Vyper, Snake const *Wutu, SaveData *const Sets);
static void GetSnakeColors(Actors* Allobj, unsigned char Mode);
static void GetGrid(Actors *Allobj, CPoint Map, short Scale);
static inline void SetApple(Actors *Allobj, Fruit *Apple, short Scale);
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
// static void WriteGameLang(void)