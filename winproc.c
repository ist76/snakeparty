// Functions for a windowed application
// Responsible for drawing the level, leaderboards, reading and writing the save file

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "snakestruct.h"
#include "winproc.h"

// Draw level and the game actors
void ActorsShow(HDC dc, cpoint const *gamemap, int scale, cpoint const *body1, int len1, cpoint const *body2, int len2, fruit const *apple)
{
     HDC memDC = CreateCompatibleDC(dc);
     HBITMAP memBM = CreateCompatibleBitmap(dc, gamemap->x * scale, gamemap->y * scale);
     SelectObject(memDC, memBM);
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor(memDC, RGB(248, 248, 224));
         Rectangle(memDC, 0, 0, gamemap->x*scale, gamemap->y*scale);
          
     // Draw cells
     SelectObject(memDC, GetStockObject(DC_PEN));
     SetDCPenColor(memDC, RGB(212, 224, 212));
     for (int i = scale; i < gamemap->x * scale; i +=scale)
     {
          MoveToEx(memDC, i, 1, NULL);
          LineTo(memDC, i, gamemap->y*scale - 1);
     }
     for (int i = scale; i < gamemap->y * scale; i +=scale)
     {
          MoveToEx(memDC, 1, i, NULL);
          LineTo(memDC, gamemap->x*scale - 1, i);
     }

     // Draw apple
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCPenColor(memDC, RGB(8, 16, 8));
     int gChan = 16;  // Reg or Gold apple
     switch (apple->price)
     {
     case ColorGold:
          SetDCBrushColor(memDC, 0x000080FF);
          break;
     case ColorBlack:
          SetDCBrushColor(memDC, 0x00080808);
          break;     
     default:
          SetDCBrushColor(memDC, 0x000000FF);
          break;
     }
     
               Ellipse(memDC, apple->coord.x*scale +2, apple->coord.y*scale +2, (apple->coord.x+1)*scale -2, (apple->coord.y+1)*scale -2);

     // Draw Snake1
     SetDCBrushColor(memDC, RGB(0, 16, 255));
     for (int i = 0; i < len1; i++)
               {                   
                    int bChan = (i <= 63) ? 255 - i*4 : 3 + (i-64)*4;  // Snake's color gradient
                    int rChan = (i <= 63) ? i*4 : 255 - (i-64)*4;
                    SetDCBrushColor(memDC, RGB(rChan, 249, bChan));
                    RoundRect(memDC, body1[i].x*scale, body1[i].y*scale,
                    (body1[i].x+1)*scale, (body1[i].y+1)*scale, scale / 4, scale / 4);
               }

     // Draw Snake2
     for (int i = 0; i < len2; i++)
               {                   
                    int gChan = (i <= 63) ? i*4 : 255 - (i-64)*4;  // Snake's color gradient
                    int rChan = (i <= 63) ? 255 - i*4 : 3 + (i-64)*4;
                    SetDCBrushColor(memDC, RGB(rChan, gChan, 249));
                    RoundRect(memDC, body2[i].x*scale, body2[i].y*scale,
                    (body2[i].x+1)*scale, (body2[i].y+1)*scale, scale / 4, scale / 4);
               }

     BitBlt(dc, 0, 0, gamemap->x*scale, gamemap->y*scale, memDC, 0, 0, SRCCOPY);     
     DeleteDC(memDC);
     DeleteObject(memBM);
}

// Drawing the score table
void ScoresShow(HDC dc, int scale, int coins, int win, HFONT font, RECT * const rt, int lang)
{
     wchar_t score[63];
     _swprintf(score, lang == 1 ? L"\nОчков:\n\n%07i\n\nПобед:\n\n%i" : L"\nScore:\n\n%07i\n\nWins:\n\n%i", coins, win, *rt);     
     HDC memDC = CreateCompatibleDC(dc);
     HBITMAP memBM = CreateCompatibleBitmap(dc, rt->right, rt->bottom);
     SelectObject(memDC, memBM);
     SelectObject(memDC, GetStockObject(DC_BRUSH));
     SetDCBrushColor(memDC, RGB(248, 248, 248));
         Rectangle(memDC, rt->left, rt->top, rt->right, rt->bottom);
     
     SelectObject(memDC, font);
     SetBkColor(memDC, RGB(248, 248, 248));
     DrawTextW(memDC, score, -1, rt, DT_CENTER); // Writes text directly to the window

     BitBlt(dc, rt->left, rt->top, rt->right, rt->bottom, memDC, 0, 0, SRCCOPY);     
     DeleteDC(memDC);
     DeleteObject(memBM);
}

// Converts keystrokes into movement direction (in game logic format)
void DispatchVector(WPARAM key, cpoint * newvect1, cpoint * newvect2, DWORD * next_tick)
{
     switch (key)
     {
     case 0x41:               // Key LEFT (A)
          newvect1->x = -1;
          newvect1->y = 0;
          break;
     
     case 0x44:               // Key RIGHT (D)
          newvect1->x = 1;
          newvect1->y = 0;
          break;
     
     case 0x57:               // Key UP (W)
          newvect1->x = 0;
          newvect1->y = -1;
          break;
     
     case 0x53:               // Key DOWN (S)
          newvect1->x = 0;
          newvect1->y = 1;
          break;

     case 0x25:               // Key LEFT
          newvect2->x = -1;
          newvect2->y = 0;
          break;
     
     case 0x27:               // Key RIGHT
          newvect2->x = 1;
          newvect2->y = 0;
          break;
     
     case 0x26:               // Key UP
          newvect2->x = 0;
          newvect2->y = -1;
          break;
     
     case 0x28:               // Key DOWN
          newvect2->x = 0;
          newvect2->y = 1;
          break;

     case 0x1B:               // Key PAUSE (ESC)
          *next_tick = *next_tick != UINT_MAX ? UINT_MAX : GetTickCount();
          break;
     
     default:
          break;
     }
}

// Handling menu item selections
void DispatchMenu(WPARAM val, cpoint * map, int * scale, int * lang)
{
     switch (val)
     {
     case 1001:
          map->x = SMALLMAPX;
          map->y = SMALLMAPY;
          break;

     case 1002:
          map->x = MEDIUMMAPX;
          map->y = MEDIUMMAPY;
          break;

     case 1003:
          map->x = LARGEMAPX;
          map->y = LARGEMAPY;
          break;

     case 1011:
          *scale = BIGSCALE;
          break;

     case 1012:
          *scale = HUGESCALE;
          break;
     
     case 1100:
          *lang = (*lang == 0) ? 1 : 0;
          break;
     
     default:
          break;
     }
}

savedata ReadSavegame() // No comments..
{
     savedata usersave = { .gamemap = {.x = MEDIUMMAPX, .y = MEDIUMMAPY},
                           .gamescale = BIGSCALE, .lang = 0};
     HANDLE hFile = CreateFile(L"snake2.sav", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
     if (INVALID_HANDLE_VALUE == hFile) return usersave; // FIXME! Write check correctness later
     ReadFile(hFile, &usersave, sizeof(usersave), NULL, NULL);
     CloseHandle(hFile);
     return usersave;
}

void WriteSavegame(cpoint maps, int scale, int lang)
{
     HANDLE hFile = CreateFile(L"snake2.sav", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
     if (INVALID_HANDLE_VALUE == hFile) return;
     savedata usersave = { .gamemap = maps,
                           .gamescale = scale,
                           .lang = lang};
     WriteFile(hFile, &usersave, sizeof(usersave), NULL, NULL);
     CloseHandle(hFile);
}

void RunAppCopy(void)
{
     wchar_t path[256];
     GetModuleFileNameW(0, path, 256); // Get full name of snake.exe
     STARTUPINFO si;  // Example of using CreateProcess from MSDN
     PROCESS_INFORMATION pi;
     ZeroMemory( &si,sizeof(si));
     si.cb = sizeof(si);
     ZeroMemory(&pi, sizeof(pi));
     CreateProcessW(path, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
     CloseHandle(pi.hProcess);
     CloseHandle(pi.hThread);
}