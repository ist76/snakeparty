/* Functions for a windowed application
   Responsible for drawing the level, leaderboards, actors */

#include <stdio.h>
#include <windows.h>
#include "snakestruct.h"
#include "winproc.h"

void DrawGrid(HDC sdc, actors const *allobj)
{
     SelectObject(sdc, GetStockObject(DC_PEN));
     SetDCPenColor(sdc, RGB(212, 224, 212));
     for (int i = 0; i < allobj->GLen; i++)
     {
          MoveToEx(sdc, allobj->Grid[i].left, allobj->Grid[i].top, NULL);
          LineTo  (sdc, allobj->Grid[i].right, allobj->Grid[i].bottom);
     }
}

void DrawSnakes(HDC sdc, actors const *allobj, int mode)
{
     int round = (allobj->ASnake[0].right - allobj->ASnake[0].left) / 4; // In order not to drag the scale, we calculate again
     for (int i = 0; i < allobj->ALen; i++)
     {
          SetDCPenColor(sdc, 0x00008000);
          SetDCBrushColor(sdc, allobj->AColor[i]);
          RoundRect(sdc, allobj->ASnake[i].left, allobj->ASnake[i].top,
                    allobj->ASnake[i].right, allobj->ASnake[i].bottom, round, round);
     }
     if (!mode) return;  // If single player

     for (int i = 0; i < allobj->BLen; i++)
     {
          SetDCPenColor(sdc, 0x00000080);
          SetDCBrushColor(sdc, allobj->BColor[i]);
          RoundRect(sdc, allobj->BSnake[i].left, allobj->BSnake[i].top,
                    allobj->BSnake[i].right, allobj->BSnake[i].bottom, round, round);
     }
}

void DrawApple(HDC sdc, actors *allobj)
{
     SelectObject(sdc, GetStockObject(DC_BRUSH));
     SetDCPenColor(sdc, RGB(8, 16, 8));
     SetDCBrushColor(sdc, allobj->AppleColor);
     Ellipse(sdc, allobj->RApple.left, allobj->RApple.top,
                  allobj->RApple.right, allobj->RApple.bottom);
}

// Draw level and the game actors
void ActorsShow(HDC dc, actors *allobj, int mode)
{
     HDC memDC = CreateCompatibleDC(dc);
     HBITMAP memBM = CreateCompatibleBitmap(dc, allobj->LevelWin.x, allobj->LevelWin.y);
     SelectObject(memDC, memBM);

     // Draw background
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor(memDC, RGB(248, 248, 224));
          Rectangle(memDC, 0, 0, allobj->LevelWin.x, allobj->LevelWin.y);

     DrawGrid(memDC, allobj);
     DrawApple(memDC, allobj);
     DrawSnakes(memDC, allobj, mode);

     BitBlt(dc, 0, 0, allobj->LevelWin.x, allobj->LevelWin.y, memDC, 0, 0, SRCCOPY);
     DeleteDC(memDC);
     DeleteObject(memBM);
}

// Drawing the score table(s)
void ScoresShow(HDC dc, snake *vyper, HFONT font, RECT * const rt, wchar_t *message, int mode)
{
     wchar_t score[63];
     mode ?
          _swprintf(score, message, vyper->coins, vyper->win, *rt) :
          _swprintf(score, message, vyper->coins, vyper->maxscore, *rt) ;
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

void SolutionShow(HDC dc, HFONT font, RECT * const rt, wchar_t *message)
{
     wchar_t score[254];
     _swprintf(score, message, *rt);
     HDC memDC = CreateCompatibleDC(dc);
     HBITMAP memBM = CreateCompatibleBitmap(dc, rt->right, rt->bottom);
     SelectObject(memDC, memBM);
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor(memDC, RGB(248, 248, 248));
         Rectangle(memDC, rt->left, rt->top, rt->right, rt->bottom);

     SelectObject(memDC, font);
     SetBkColor(memDC, RGB(248, 248, 248));
     DrawTextW(memDC, score, -1, rt, DT_CENTER);

     BitBlt(dc, rt->left, rt->top, rt->right, rt->bottom, memDC, 0, 0, SRCCOPY);
     DeleteDC(memDC);
     DeleteObject(memBM);
}