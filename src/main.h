// Declarations for main()

int SnakeLogic(savedata const *game, fruit *apple, int *ticks, snake *vyper, snake * wutu);
void SnakeRestart(savedata const *game, snake *vyper, snake *wutu, int *ticks, fruit *apple);
void ActorsShow(HDC dc, actors *allobj, int mode);
void ScoresShow(HDC dc, snake *vyper, HFONT font, RECT * const rt, wchar_t *message, int mode);
void SolutionShow(HDC dc, HFONT font, RECT * const rt, wchar_t *message);