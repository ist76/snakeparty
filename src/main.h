/* Declarations for main() */

#define INPUTLAG 1

int  SnakeLogic(SaveData const *Game, Fruit *Apple, int *Ticks, Snake *Vyper, Snake *Wutu, unsigned int *State);
void SnakeRestart(SaveData const *Game, Fruit *Apple, int *Ticks, Snake *Vyper, Snake *Wutu, unsigned int *State);
void ActorsShow(HWND Window, Actors *AllObj);
void SolutionShow(HWND Window, HFONT Font, RECT * const rt, wchar_t *Message);
void DrawInterruption(HWND Window, Actors *AllObj);

static void MakeMenu(HWND hwnd, SaveData *GSets, GameLang *Marks);
static void GetSnakesCells(Actors *Allobj, Snake const *Vyper, Snake const *Wutu, SaveData *const Sets);
static void RunAppCopy(void);
static void SetApple(Actors *Allobj, Fruit *Apple, short Scale);
static void SetInfo(wchar_t *Score, GameLang *Marks, Snake *Vyper, SaveData *Sets);
static void        GetSnakeColors(Actors* Allobj, unsigned char Mode);
static void        GetGrid(Actors *Allobj, CPoint Map, short Scale);
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
// static void WriteGameLang(void)