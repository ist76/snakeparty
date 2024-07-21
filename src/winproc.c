// Functions for a windowed application
// Responsible for drawing the level, leaderboards, actors

#include <stdio.h>
#include <windows.h>
#include "snakestruct.h"
#include "winproc.h"

// Calculate Snake's coords before rendering
void GetSnakeCells(RECT *SnakeCells, cpoint const *body, int len, int scale)
{
     for (int i = 0; i < len; i++)
     {
          SnakeCells[i].left    = body[i].x * scale;
          SnakeCells[i].top     = body[i].y * scale;
          SnakeCells[i].right   = (body[i].x + 1) * scale;
          SnakeCells[i].bottom  = (body[i].y + 1) * scale;
     }
}

// Calculate Snake's cells color gradient (one time)
void GetSnakeColors(actors *allobj)
{
     for (int i = 0; i <= 31; i++)
     {
          allobj->AColor[i] = allobj->AColor[126 - i] = allobj->AColor[126 + i] = allobj->AColor[253 - i]
                            = RGB(i * 4, 249, 255 - i * 4);
          allobj->BColor[i] = allobj->BColor[126 - i] = allobj->BColor[126 + i] = allobj->BColor[253 - i]
                            = RGB(240, i * 4, 255 - i * 2);
     }
}

// Calculate grid lines only one time, use every 16ms
void GetGrid(actors *allobj, cpoint map, int scale)
{
     int counter = 0;
     for (int i = 0; i < map.x; i++)
     {
          allobj->Grid[counter].left   = (i+1) * scale;
          allobj->Grid[counter].top    = 1;
          allobj->Grid[counter].right  = (i+1) * scale;
          allobj->Grid[counter].bottom = map.y * scale - 1;
          counter++;
     }
     for (int i = 0; i < map.y; i++)
     {
          allobj->Grid[counter].left   = 1;
          allobj->Grid[counter].top    = (i+1) * scale;
          allobj->Grid[counter].right  = map.x * scale - 1;
          allobj->Grid[counter].bottom = (i+1) * scale;
          counter++;
     }
     allobj->GLen = counter;
}

// Calculate apple coordinates & color
void SetApple(actors *allobj, fruit *apple, int scale)
{
     switch (apple->price)
     {
     case ColorGold:
          allobj->AppleColor = 0x000080FF;
          break;

     case ColorBlack:
          allobj->AppleColor = 0x00080808;
          break;

     default:
          allobj->AppleColor = 0x000000FF;
          break;
     }
     allobj->RApple.left   =  apple->coord.x*scale + 2;
     allobj->RApple.top    =  apple->coord.y*scale + 2;
     allobj->RApple.right  = (apple->coord.x + 1) * scale - 2;
     allobj->RApple.bottom = (apple->coord.y + 1) * scale - 2;
}

void DrawSnake(HDC sdc, RECT const *body, int len, int *rectcolor)
{
     SetDCBrushColor(sdc, RGB(0, 16, 255));
     int round = (body[0].right - body[0].left) / 4; // In order not to drag the scale, we calculate again
     for (int i = 0; i < len; i++)
     {
          SetDCBrushColor(sdc, rectcolor[i]);
          RoundRect(sdc, body[i].left, body[i].top,
                    body[i].right, body[i].bottom, round, round);
     }
}

void DrawGrid(HDC sdc, RECT const *grid, int gridlen)
{
     SelectObject(sdc, GetStockObject(DC_PEN));
     SetDCPenColor(sdc, RGB(212, 224, 212));
     for (int i = 0; i < gridlen; i++)
     {
          MoveToEx(sdc, grid[i].left, grid[i].top, NULL);
          LineTo  (sdc, grid[i].right, grid[i].bottom);
     }
}

void DrawApple(HDC sdc, actors allobj)
{
     SelectObject(sdc, GetStockObject(DC_BRUSH));
     SetDCPenColor(sdc, RGB(8, 16, 8));
     SetDCBrushColor(sdc, allobj.AppleColor);
     Ellipse(sdc, allobj.RApple.left, allobj.RApple.top, allobj.RApple.right, allobj.RApple.bottom);
}

// Draw level and the game actors
void ActorsShow(HDC dc, actors *allobj, fruit const *apple)
{
     HDC memDC = CreateCompatibleDC(dc);
     HBITMAP memBM = CreateCompatibleBitmap(dc, allobj->LewelWin.x, allobj->LewelWin.y);
     SelectObject(memDC, memBM);

     // Draw background
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor(memDC, RGB(248, 248, 224));
          Rectangle(memDC, 0, 0, allobj->LewelWin.x, allobj->LewelWin.y);

     DrawGrid(memDC, allobj->Grid, allobj->GLen);
     DrawApple(memDC, *allobj);
     DrawSnake(memDC, allobj->ASnake, allobj->ALen, allobj->AColor);
     DrawSnake(memDC, allobj->BSnake, allobj->BLen, allobj->BColor);

     BitBlt(dc, 0, 0, allobj->LewelWin.x, allobj->LewelWin.y, memDC, 0, 0, SRCCOPY);
     DeleteDC(memDC);
     DeleteObject(memBM);
}

// Drawing the score table(s)
void ScoresShow(HDC dc, int coins, int win, HFONT font, RECT * const rt, int lang)
{
     wchar_t score[63];
     _swprintf(score, lang ? L"\nОчков:\n\n%07i\n\nПобед:\n\n%i"
                           : L"\nScore:\n\n%07i\n\nWins:\n\n%i", coins, win, *rt);
     HDC memDC = CreateCompatibleDC(dc);
     HBITMAP memBM = CreateCompatibleBitmap(dc, rt->right, rt->bottom);
     SelectObject(memDC, memBM);
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor(memDC, RGB(248, 248, 248));
         Rectangle(memDC, rt->left, rt->top, rt->right, rt->bottom);

     SelectObject(memDC, font);
     SetBkColor(memDC, RGB(248, 248, 248));
     DrawTextW(memDC, score, -1, rt, DT_CENTER); // Write text directly to the window

     BitBlt(dc, rt->left, rt->top, rt->right, rt->bottom, memDC, 0, 0, SRCCOPY);
     DeleteDC(memDC);
     DeleteObject(memBM);
}