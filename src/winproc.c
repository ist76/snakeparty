/* Functions for a windowed application
   Responsible for drawing the level, leaderboards, actors */

#include <windows.h>
#include "snakestruct.h"
#include "winproc.h"

static inline void DrawGrid(HDC Sdc, Actors const *AllObj)
{
     SelectObject    (Sdc, GetStockObject(DC_PEN));
     SetDCPenColor   (Sdc, RGB(212, 224, 212));
     for (int i = 0; i < AllObj->GLen; i++)
     {
          MoveToEx   (Sdc, AllObj->Grid[i].left, AllObj->Grid[i].top, NULL);
          LineTo     (Sdc, AllObj->Grid[i].right, AllObj->Grid[i].bottom);
     }
}

static inline void DrawSnakes(HDC Sdc, Actors const *AllObj)
{
     int round = (AllObj->ASnake[0].right - AllObj->ASnake[0].left) / 4; // In order not to drag the scale, we calculate again
     for (short i = 0; i < AllObj->ALen; i++)
     {
          SetDCPenColor    (Sdc, 0x00008000);
          SetDCBrushColor  (Sdc, AllObj->AColor[i]);
          RoundRect        (Sdc, AllObj->ASnake[i].left, AllObj->ASnake[i].top,
                            AllObj->ASnake[i].right, AllObj->ASnake[i].bottom, round, round);
     }
     if (!AllObj->BLen) return;  // If single player

     for (short i = 0; i < AllObj->BLen; i++)
     {
          SetDCPenColor    (Sdc, 0x00000080);
          SetDCBrushColor  (Sdc, AllObj->BColor[i]);
          RoundRect        (Sdc, AllObj->BSnake[i].left, AllObj->BSnake[i].top,
                            AllObj->BSnake[i].right, AllObj->BSnake[i].bottom, round, round);
     }
}

static inline void DrawApple(HDC Sdc, Actors *AllObj)
{
     SelectObject    (Sdc, GetStockObject(DC_BRUSH));
     SetDCPenColor   (Sdc, RGB(8, 16, 8));
     SetDCBrushColor (Sdc, AllObj->AppleColor);
     Ellipse         (Sdc, AllObj->RApple.left, AllObj->RApple.top,
                      AllObj->RApple.right, AllObj->RApple.bottom);
}

// Draw level and the game Actors
void ActorsShow(HWND Window, Actors *AllObj)
{
     HDC Dc        = GetDC(Window);
     HDC memDC     = CreateCompatibleDC(Dc);
     HBITMAP memBM = CreateCompatibleBitmap(Dc, AllObj->LevelWin.x, AllObj->LevelWin.y);
     SelectObject    (memDC, memBM);

     // Draw background
     SelectObject    (memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor (memDC, RGB(248, 248, 224));
          Rectangle  (memDC, 0, 0, AllObj->LevelWin.x, AllObj->LevelWin.y);

     DrawGrid        (memDC, AllObj);
     DrawApple       (memDC, AllObj);
     DrawSnakes      (memDC, AllObj);

     BitBlt          (Dc, 0, 0, AllObj->LevelWin.x, AllObj->LevelWin.y, memDC, 0, 0, SRCCOPY);
     DeleteDC        (memDC);
     DeleteObject    (memBM);
     ReleaseDC       (Window, Dc);
}

void DrawInterruption(HWND Window, Actors *AllObj)
{
     HDC Dc        = GetDC(Window);
     HDC memDC     = CreateCompatibleDC(Dc);
     HBITMAP memBM = CreateCompatibleBitmap(Dc, AllObj->LevelWin.x, AllObj->LevelWin.y);
     SelectObject    (memDC, memBM);

     // Draw background
     SelectObject    (memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor (memDC, RGB(8, 32, 32));
          Rectangle  (memDC, 0, 0, AllObj->LevelWin.x, AllObj->LevelWin.y);
     SelectObject    (memDC, GetStockObject(DC_PEN));
     SetDCPenColor   (memDC, RGB(8, 192, 64));
     for (short i = 36; i < AllObj->LevelWin.x; i += 36)
     {
          MoveToEx   (memDC, i, 1, NULL);
          LineTo     (memDC, i, AllObj->LevelWin.y - 1);
          BitBlt     (Dc, 0, 0, AllObj->LevelWin.x, AllObj->LevelWin.y, memDC, 0, 0, SRCCOPY);
          Sleep(1);
     }
     for (short j = AllObj->LevelWin.y - 36; j > 0; j -= 36)
     {
          MoveToEx   (memDC, 1, j, NULL);
          LineTo     (memDC, AllObj->LevelWin.x - 1, j);
          BitBlt     (Dc, 0, 0, AllObj->LevelWin.x, AllObj->LevelWin.y, memDC, 0, 0, SRCCOPY);
          Sleep(1);
     }
     DeleteDC        (memDC);
     DeleteObject    (memBM);
     ReleaseDC       (Window, Dc);
}

// Draw scores for both players or solution
void SolutionShow(HWND Window, HFONT font, RECT * const rt, wchar_t *message)
{
     HDC Dc        = GetDC(Window);
     HDC memDC     = CreateCompatibleDC(Dc);
     HBITMAP memBM = CreateCompatibleBitmap(Dc, rt->right, rt->bottom);
     SelectObject    (memDC, memBM);
     SelectObject    (memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor (memDC, RGB(248, 248, 248));
         Rectangle   (memDC, rt->left, rt->top, rt->right, rt->bottom);

     SelectObject    (memDC, font);
     SetBkColor      (memDC, RGB(248, 248, 248));
     DrawTextW       (memDC, message, -1, rt, DT_CENTER);

     BitBlt          (Dc, rt->left, rt->top, rt->right, rt->bottom, memDC, 0, 0, SRCCOPY);
     DeleteDC        (memDC);
     DeleteObject    (memBM);
     ReleaseDC       (Window, Dc);
}