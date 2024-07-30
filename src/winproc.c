/* Functions for a windowed application
   Responsible for drawing the level, leaderboards, actors */

#include <stdio.h>
#include <windows.h>
#include "snakestruct.h"
#include "winproc.h"

static inline void DrawGrid(HDC sdc, Actors const *allobj)
{
     SelectObject(sdc, GetStockObject(DC_PEN));
     SetDCPenColor(sdc, RGB(212, 224, 212));
     for (int i = 0; i < allobj->GLen; i++)
     {
          MoveToEx(sdc, allobj->Grid[i].left, allobj->Grid[i].top, NULL);
          LineTo  (sdc, allobj->Grid[i].right, allobj->Grid[i].bottom);
     }
}

static inline void DrawSnakes(HDC sdc, Actors const *allobj, unsigned char mode)
{
     int round = (allobj->ASnake[0].right - allobj->ASnake[0].left) / 4; // In order not to drag the scale, we calculate again
     for (short i = 0; i < allobj->ALen; i++)
     {
          SetDCPenColor(sdc, 0x00008000);
          SetDCBrushColor(sdc, allobj->AColor[i]);
          RoundRect(sdc, allobj->ASnake[i].left, allobj->ASnake[i].top,
                    allobj->ASnake[i].right, allobj->ASnake[i].bottom, round, round);
     }
     if (!mode) return;  // If single player

     for (short i = 0; i < allobj->BLen; i++)
     {
          SetDCPenColor(sdc, 0x00000080);
          SetDCBrushColor(sdc, allobj->BColor[i]);
          RoundRect(sdc, allobj->BSnake[i].left, allobj->BSnake[i].top,
                    allobj->BSnake[i].right, allobj->BSnake[i].bottom, round, round);
     }
}

static inline void DrawApple(HDC sdc, Actors *allobj)
{
     SelectObject(sdc, GetStockObject(DC_BRUSH));
     SetDCPenColor(sdc, RGB(8, 16, 8));
     SetDCBrushColor(sdc, allobj->AppleColor);
     Ellipse(sdc, allobj->RApple.left, allobj->RApple.top,
                  allobj->RApple.right, allobj->RApple.bottom);
}

// Draw level and the game Actors
void ActorsShow(HDC dc, Actors *allobj, unsigned char mode)
{
     HDC memDC = CreateCompatibleDC(dc);
     HBITMAP memBM = CreateCompatibleBitmap(dc, allobj->LevelWin.x, allobj->LevelWin.y);
     SelectObject(memDC, memBM);

     // Draw background
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor(memDC, RGB(248, 248, 224));
          Rectangle(memDC, 0, 0, allobj->LevelWin.x, allobj->LevelWin.y);

     DrawGrid   (memDC, allobj);
     DrawApple  (memDC, allobj);
     DrawSnakes (memDC, allobj, mode);

     BitBlt       (dc, 0, 0, allobj->LevelWin.x, allobj->LevelWin.y, memDC, 0, 0, SRCCOPY);
     DeleteDC     (memDC);
     DeleteObject (memBM);
}

// Drawing the score table(s)
void ScoresShow(HDC dc, Snake *vyper, HFONT font, RECT * const rt, wchar_t *message, unsigned char mode)
{
     wchar_t score[63];
     mode ?
          swprintf_s(score, 63, message, vyper->Coins, vyper->Win) :
          swprintf_s(score, 63, message, vyper->Coins, vyper->MaxScore) ;
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
     HDC memDC = CreateCompatibleDC(dc);
     HBITMAP memBM = CreateCompatibleBitmap(dc, rt->right, rt->bottom);
     SelectObject(memDC, memBM);
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor(memDC, RGB(248, 248, 248));
         Rectangle(memDC, rt->left, rt->top, rt->right, rt->bottom);

     SelectObject(memDC, font);
     SetBkColor(memDC, RGB(248, 248, 248));
     DrawTextW(memDC, message, -1, rt, DT_CENTER);

     BitBlt(dc, rt->left, rt->top, rt->right, rt->bottom, memDC, 0, 0, SRCCOPY);
     DeleteDC(memDC);
     DeleteObject(memBM);
}