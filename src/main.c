/* Launching the application, drawing windows and the main loop - here
   For project build  in MinGW64 with UTF-8 support, need use wchar_t types, w-functions, _swprintf
   instead of sprintf, 'L'-prefix for all strings and $gcc ... '-municode'
   I'm sure there is another, simpler and correct way, but I don't know it yet */

#include <stdio.h>
#include <windows.h>
#include "snakestruct.h"
#include "winproc.h"
#include "main.h"
#include "winio.h"

/* An example of a function that creates a file with a localized interface.
   Just write your lines in the appropriate positions and run the function
   (See winio.h for details) */
/*
static void WriteGameLang(void)
{
     gamelang userlang = { .str1001 = L"Малый",        .str1002 = L"Средний",        .str1003 = L"Большой",
                           .str1008 = L"Размер карты", .str1009 = L"Масштаб",        .str1011 = L"Крупный",
                           .str1012 = L"Огромный",     .str1013 = L"Один игрок",     .str1014 = L"Два игрока",
                           .str1100 = L"Язык",         .str1110 = L"Режим игры",
                           .str1501 = L"\nОчков:\n\n%07i\n\nПобед:\n\n%i",
                           .str1502 = L"\nОчков:\n\n%07i\n\nРекорд\n\n%07i",
                           .str1503 = L"\nВыберите направление \nчтобы играть\n\nУправление: \nстрелки или\n«W S A D»\nна клавиатуре\n\nEscape - пауза\n\n\nИзменение любых\nнастроек требует\nперезапуска\n",
                         };
     HANDLE hFile = CreateFile(L"snake.lng", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
     if (INVALID_HANDLE_VALUE == hFile) return;
     WriteFile(hFile, &userlang, sizeof(userlang), NULL, NULL);
     CloseHandle(hFile);
}
*/

// Calculate Snake's coords before rendering
static inline void GetSnakesCells(actors *allobj, snake const *vyper, snake const *wutu, size_t scale, size_t mode)
{
     allobj->ALen = vyper->len;
     for (size_t i = 0; i < vyper->len; i++)
     {
          allobj->ASnake[i].left   =  vyper->body[i].x * scale;
          allobj->ASnake[i].top    =  vyper->body[i].y * scale;
          allobj->ASnake[i].right  = (vyper->body[i].x + 1) * scale;
          allobj->ASnake[i].bottom = (vyper->body[i].y + 1) * scale;
     }
     if (!mode) return;  // Single Player

     allobj->BLen = wutu->len;
     for (size_t i = 0; i < wutu->len; i++)
     {
          allobj->BSnake[i].left   =  wutu->body[i].x * scale + 1;
          allobj->BSnake[i].top    =  wutu->body[i].y * scale + 1;
          allobj->BSnake[i].right  = (wutu->body[i].x + 1) * scale + 1;
          allobj->BSnake[i].bottom = (wutu->body[i].y + 1) * scale + 1;
     }
}

// Calculate Snake's cells color gradient (one time)
static void GetSnakeColors(actors *allobj, unsigned char mode)
{
     for (size_t i = 0; i <= 63; i++)
     {
          allobj->AColor[i] = allobj->AColor[126 - i] = allobj->AColor[126 + i] =
                              allobj->AColor[253 - i] = RGB(i * 4, 249, 255 - i * 4);
          if (mode)
          {
               allobj->BColor[i] = allobj->BColor[126 - i] = allobj->BColor[126 + i] =
                                   allobj->BColor[253 - i] = RGB(191 + i, i * 4, 255 - i);
          }
     }
}

// Calculate grid lines only one time, use every 16ms
static void GetGrid(actors *allobj, cpoint map, size_t scale)
{
     int counter = 0;
     for (size_t i = 0; i < map.x; i++)
     {
          allobj->Grid[counter].left   = (i+1) * scale;
          allobj->Grid[counter].top    = 1;
          allobj->Grid[counter].right  = (i+1) * scale;
          allobj->Grid[counter].bottom = map.y * scale - 1;
          counter++;
     }
     for (size_t i = 0; i < map.y; i++)
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
static inline void SetApple(actors *allobj, fruit *apple, size_t scale)
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

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
     if (message == WM_DESTROY) PostQuitMessage(0);
     return DefWindowProc(hwnd, message, wparam, lparam);
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
     savedata gsets = ReadSavegame();                             // Get previously saved data
     int GameTicks;                                               // Latency (ms) between game loops
     snake anaconda   = {.win = 0, .maxscore = gsets.maxs};       // First snake
     snake bushmaster = {.win = 0};                               // Second snake
     actors AllActors;                                            // For render level, look at winproc.h
            AllActors.LevelWin.x = gsets.map.x * gsets.scale;     // Set game level size in px
            AllActors.LevelWin.y = gsets.map.y * gsets.scale;
     RECT ScoreTable;                                             // Size of score table
     SetRect(&ScoreTable, 0, 0, 7 * gsets.scale,
             (gsets.map.x/3 - 1) * gsets.scale + gsets.scale/2);
     GetGrid(&AllActors, gsets.map, gsets.scale);                 // Array of points to draw the frid
     GetSnakeColors(&AllActors, gsets.mode);
     gamelang marks = ReadGamelang(gsets.lang);
     // WriteGameLang();  //  This function call generate snake.lng file, if you need to create custom translate

     /* Create all GUI */
     WNDCLASSW wcl;
         memset(&wcl, 0, sizeof(WNDCLASSW));
         wcl.lpszClassName = L"mainwin";
         wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
         wcl.style = CS_GLOBALCLASS;
         wcl.lpfnWndProc = WndProc;
     RegisterClassW(&wcl);

     HWND hwnd = CreateWindowW(L"mainwin", L"Snake Party", 0x100A0000, 8, 8,
                              (AllActors.LevelWin.x + ScoreTable.right + (3 * gsets.scale) / 2 + 2),
                              (AllActors.LevelWin.y + (5 * gsets.scale) / 2 - 2), NULL, NULL, NULL, NULL);

     // Separate window with game level
     HWND game_map = CreateWindowW(L"static", NULL, WS_VISIBLE | WS_CHILD , gsets.scale/2 - 2,
                                   gsets.scale/2, AllActors.LevelWin.x, AllActors.LevelWin.y, hwnd, NULL, NULL, NULL);

     // Make Scoreboard 1
     HWND scores1 = CreateWindowW(L"static", NULL, SS_CENTER | WS_VISIBLE | WS_CHILD , (gsets.map.x+1)*gsets.scale,
                                  gsets.scale/2, ScoreTable.right, ScoreTable.bottom, hwnd, NULL, NULL, NULL);

     // Make Scoreboard 2
     HWND scores2 = CreateWindowW(L"static", NULL, SS_CENTER | WS_VISIBLE | WS_CHILD , (gsets.map.x+1)*gsets.scale,
                                  ScoreTable.bottom + gsets.scale + gsets.scale/2,
                                  ScoreTable.right, ScoreTable.bottom, hwnd, NULL, NULL, NULL);

     // Font for scoreboards
     HFONT hFont = CreateFontW(AllActors.LevelWin.y / 20, 0, 0, 0, 500, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                               OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, FALSE);

     HFONT hFontS = CreateFontW(AllActors.LevelWin.y / 34, 0, 0, 0, 500, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                               OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, FALSE);


     // Make main menu
     HMENU MenuBar = CreateMenu();
     HMENU hPMenu1 = CreatePopupMenu();
     HMENU hPMenu2 = CreatePopupMenu();
     HMENU hPMenu3 = CreatePopupMenu();

     AppendMenuW(MenuBar, MF_STRING | MF_POPUP, (UINT_PTR)hPMenu1, marks.str1008);
     AppendMenuW(MenuBar, MF_STRING | MF_POPUP, (UINT_PTR)hPMenu2, marks.str1009);
     AppendMenuW(MenuBar, MF_STRING | MF_POPUP, (UINT_PTR)hPMenu3, marks.str1110);
     AppendMenuW(MenuBar, MF_STRING,                         1100, marks.str1100);
     AppendMenuW(hPMenu1, MF_STRING | (gsets.map.x == SMALLMAPX  ? MF_CHECKED : MF_UNCHECKED), 1001, marks.str1001);
     AppendMenuW(hPMenu1, MF_STRING | (gsets.map.x == MEDIUMMAPX ? MF_CHECKED : MF_UNCHECKED), 1002, marks.str1002);
     AppendMenuW(hPMenu1, MF_STRING | (gsets.map.x == LARGEMAPX  ? MF_CHECKED : MF_UNCHECKED), 1003, marks.str1003);
     AppendMenuW(hPMenu2, MF_STRING | (gsets.scale == BIGSCALE   ? MF_CHECKED : MF_UNCHECKED), 1011, marks.str1011);
     AppendMenuW(hPMenu2, MF_STRING | (gsets.scale == HUGESCALE  ? MF_CHECKED : MF_UNCHECKED), 1012, marks.str1012);
     AppendMenuW(hPMenu3, MF_STRING | (!gsets.mode               ? MF_CHECKED : MF_UNCHECKED), 1013, marks.str1013);
     AppendMenuW(hPMenu3, MF_STRING | (gsets.mode                ? MF_CHECKED : MF_UNCHECKED), 1014, marks.str1014);

     SetMenu(hwnd, MenuBar);
     SetMenu(hwnd, hPMenu1);
     SetMenu(hwnd, hPMenu2);
     /* End of creating GUI */

     srand(GetTickCount());                                             // For generate Apple
     fruit apple;                                                       // Make empty Apple struct
     SnakeRestart(&gsets, &anaconda, &bushmaster, &GameTicks, &apple);  // Game first initialization
     DWORD next_game_tick = GetTickCount();                             // Timer for game loop
     DWORD next_render_tick = GetTickCount();                           // Timer for render loop
     MSG msg;                                                           // Messages from app
     HDC dc;                                                            // Temporary context

     for(;(anaconda.len < 253) || (bushmaster.len < 253);) // Main Game loop. Remember the snake.body[254]
     {
          if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
          {
              // Processing keyboard commands
              if (msg.message == WM_KEYDOWN) DispatchVector(msg.wParam, &anaconda.newvectr, &bushmaster.newvectr,
                                                            &next_game_tick, gsets.mode);

              // When you select a menu item, the settings are overwritten and the application is restarted
              if (msg.message == WM_COMMAND)
              {
                   DispatchMenu(msg.wParam, &gsets);
                   WriteSavegame(&gsets, anaconda.maxscore);
                   RunAppCopy();  // Try restart the application
                   break;
              }

              // Autosave on exit
              if (msg.message == WM_QUIT)
              {
                   WriteSavegame(&gsets, anaconda.maxscore);
                   break;
              }
              DispatchMessageW(&msg);
          }

          Sleep(1); // Sleep, save the Battery!

          while(GetTickCount() > next_game_tick)
          {
              if (!gsets.mode)  // if singleplayer
              {
                    if (!SnakeLogic(&gsets, &apple, &GameTicks, &anaconda, &bushmaster))  // Snake suicide?
                         {
                          SnakeRestart(&gsets, &anaconda, &bushmaster, &GameTicks, &apple);
                         }
                    dc = GetDC(scores2);  // Draw solution in scores-2 window
                    SolutionShow(dc, hFontS, &ScoreTable, marks.str1503);
                    ReleaseDC(scores2, dc);
              }

              else
              {
                    if (!SnakeLogic(&gsets, &apple, &GameTicks, &anaconda, &bushmaster) ||
                        !SnakeLogic(&gsets, &apple, &GameTicks, &bushmaster, &anaconda))
                        {
                         SnakeRestart(&gsets, &anaconda, &bushmaster, &GameTicks, &apple);
                        }
                    dc = GetDC(scores2);  // Draw scores in scores-2 window
                    ScoresShow(dc, &bushmaster, hFont, &ScoreTable, marks.str1501, gsets.mode);
                    ReleaseDC(scores2, dc);
              }

              // We calculate the coordinates of all actors not every 16ms, but only ever GameTick
              SetApple(&AllActors, &apple, gsets.scale);
              GetSnakesCells(&AllActors, &anaconda, &bushmaster, gsets.scale, gsets.mode);
              dc = GetDC(scores1);  // Draw scores in scores-1 window
              ScoresShow(dc, &anaconda, hFont, &ScoreTable, gsets.mode ?
                         marks.str1501 : marks.str1502, gsets.mode);
              ReleaseDC(scores1, dc);
              next_game_tick += GameTicks;
          }

          while(GetTickCount() > next_render_tick)
          {
              dc = GetDC(game_map);  // Draw level and actors
              ActorsShow(dc, &AllActors, gsets.mode);
              ReleaseDC(game_map, dc);
              next_render_tick += RENDERLAG;
          }
     }
     return 0;
}