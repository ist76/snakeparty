// Declarations for main()

void SnakeLogic(cpoint const *gamemap, fruit *apple, int *ticks, snake *vyper, snake * wutu);
void SnakeRestart(cpoint const *gamemap, snake *vyper, snake *wutu, int *ticks, fruit *apple);
void GetSnakeCells(RECT *SnakeCells, cpoint const *body, int len, int scale);
void GetSnakeColors(actors *allobj);
void GetGrid(actors *allobj, cpoint map, int scale);
void SetApple(actors *allobj, fruit *apple, int scale);
void ActorsShow(HDC dc, actors *allobj, fruit const *apple);
void ScoresShow(HDC dc, int coins, int win, HFONT font, RECT * const rt, int lang);
fruit GetFruit(cpoint const *gamemap, snake *vyper, snake * wutu);