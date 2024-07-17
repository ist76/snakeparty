// Launching the application, drawing windows and the main loop - here
// For project build  in MinGW64 with UTF-8 support, need use wchar_t types, w-functions, _swprintf
// instead of sprintf, 'L'-prefix for all strings and $gcc ... '-municode'
// I'm sure there is another, simpler and correct way, but I don't know it yet

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "snakestruct.h"
#include "winproc.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
     if (message == WM_DESTROY) PostQuitMessage(0);
     return DefWindowProc(hwnd, message, wparam, lparam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
     savedata gsets = ReadSavegame();                                                                                 // Get previously saved data
     int GameTicks;                                                                                                   // Latency (ms) between game loops
     snake anaconda = {.win = 0};                                                                                     // First snake
     snake bushmaster = {.win = 0};                                                                                   // Second snake
     RECT ScoreTable;                                                                                                 // Size of score table
     SetRect(&ScoreTable, 0, 0, 7 * gsets.gamescale, (gsets.gamemap.x/3 - 1) * gsets.gamescale + gsets.gamescale/2);
     
     // Very very stupid translate section!!!
     wchar_t str1008[13];
     _swprintf(str1008, gsets.lang == 1 ? L"Размер карты" : L"Map size");
     wchar_t str1009[8];
     _swprintf(str1009, gsets.lang == 1 ? L"Масштаб" : L"Scale");
     wchar_t str1100[9];
     _swprintf(str1100, gsets.lang == 1 ? L"Ру -> En" : L"En -> Ру");
     wchar_t str1001[6];
     _swprintf(str1001, gsets.lang == 1 ? L"Малый" : L"Small");
     wchar_t str1002[8];
     _swprintf(str1002, gsets.lang == 1 ? L"Средний" : L"Medium");
     wchar_t str1003[8];
     _swprintf(str1003, gsets.lang == 1 ? L"Большой" : L"Large");
     wchar_t str1011[8];
     _swprintf(str1011, gsets.lang == 1 ? L"Крупный" : L"Big");
     wchar_t str1012[9];
     _swprintf(str1012, gsets.lang == 1 ? L"Огромный" : L"Huge");
     // End of very stupid translate section

     WNDCLASSW wcl;
         memset(&wcl, 0, sizeof(WNDCLASSW));
         wcl.lpszClassName = L"mainwin";
         wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
         wcl.style = CS_GLOBALCLASS;
         wcl.lpfnWndProc = WndProc;
     
     RegisterClassW(&wcl);

     HWND hwnd = CreateWindowW(L"mainwin", L"Snake Party", WS_OVERLAPPEDWINDOW&(~WS_MAXIMIZEBOX)&(~WS_THICKFRAME) | WS_VISIBLE,
                               10, 10, ((gsets.gamemap.x+9)*gsets.gamescale - gsets.gamescale/2),
                               (gsets.gamemap.y+3)*gsets.gamescale - gsets.gamescale/2, NULL, NULL, NULL, NULL);
     
     // Separate window with game level
     HWND game_map = CreateWindowW(L"static", NULL, WS_VISIBLE | WS_CHILD , gsets.gamescale/2,
                                   gsets.gamescale/2, gsets.gamemap.x*gsets.gamescale, gsets.gamemap.y*gsets.gamescale, hwnd, NULL, NULL, NULL);    
     
     // Font for scoreboardы
     HFONT hFont = CreateFontW((gsets.gamemap.y * gsets.gamescale) / 20, 0, 0, 0, 500, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
                               OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, FALSE);     
     // Make Scoreboard 1
     HWND scores1 = CreateWindowW(L"static", NULL, SS_CENTER | WS_VISIBLE | WS_CHILD , (gsets.gamemap.x+1)*gsets.gamescale, gsets.gamescale/2,
                                  ScoreTable.right, ScoreTable.bottom, hwnd, NULL, NULL, NULL);

     // Make Scoreboard 2
     HWND scores2 = CreateWindowW(L"static", NULL, SS_CENTER | WS_VISIBLE | WS_CHILD , (gsets.gamemap.x+1)*gsets.gamescale,
                                  ScoreTable.bottom + 1 * gsets.gamescale + gsets.gamescale/2,
                                  ScoreTable.right, ScoreTable.bottom, hwnd, NULL, NULL, NULL);

     // Make main menu
     HMENU MenuBar = CreateMenu();
          HMENU hPopMenu1 = CreatePopupMenu();
          HMENU hPopMenu2 = CreatePopupMenu();

          AppendMenuW(MenuBar,   MF_STRING | MF_POPUP, (UINT_PTR)hPopMenu1, str1008);
          AppendMenuW(MenuBar,   MF_STRING | MF_POPUP, (UINT_PTR)hPopMenu2, str1009);
          AppendMenuW(MenuBar,   MF_STRING,                           1100, str1100);
            
          AppendMenuW(hPopMenu1, MF_STRING | (gsets.gamemap.x    == SMALLMAPX  ? MF_CHECKED : MF_UNCHECKED), 1001, str1001);
          AppendMenuW(hPopMenu1, MF_STRING | (gsets.gamemap.x    == MEDIUMMAPX ? MF_CHECKED : MF_UNCHECKED), 1002, str1002);
          AppendMenuW(hPopMenu1, MF_STRING | (gsets.gamemap.x    == LARGEMAPX  ? MF_CHECKED : MF_UNCHECKED), 1003, str1003);
          AppendMenuW(hPopMenu2, MF_STRING | (gsets.gamescale    == BIGSCALE   ? MF_CHECKED : MF_UNCHECKED), 1011, str1011);
          AppendMenuW(hPopMenu2, MF_STRING | (gsets.gamescale    == HUGESCALE  ? MF_CHECKED : MF_UNCHECKED), 1012, str1012);

          SetMenu(hwnd, MenuBar);
          SetMenu(hwnd, hPopMenu1);
          SetMenu(hwnd, hPopMenu2);
     
     srand(GetTickCount());                                                                                   // For generate Apple
     SnakeRestart(&gsets.gamemap, &anaconda, &bushmaster, &GameTicks);                                        // Game initialization
     fruit apple = GetFruit(&gsets.gamemap, &anaconda.len,anaconda.body, &bushmaster.len, bushmaster.body);   // And the creation of an apple
     DWORD next_game_tick = GetTickCount();                                                                   // Timer for game loop
     DWORD next_render_tick = GetTickCount();                                                                 // Timer for render loop
     MSG msg;                                                                                                 // Messages from app
     
     for(;(anaconda.len < 253) || (bushmaster.len < 253);) // Main Game loop. Remember the snake.body[254]? We don't need to go beyond the array
     {
          if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
          {
              if (msg.message == WM_QUIT)
              {
                   WriteSavegame(gsets.gamemap, gsets.gamescale, gsets.lang);
                   break;  // Autosave on exit
              }
              if (msg.message == WM_KEYDOWN) DispatchVector(msg.wParam, &anaconda.newvectr, &bushmaster.newvectr, &next_game_tick);
              if (msg.message == WM_COMMAND)
              {
                   DispatchMenu(msg.wParam, &gsets.gamemap, &gsets.gamescale, &gsets.lang);
                   SnakeRestart(&gsets.gamemap, &anaconda, &bushmaster, &GameTicks);
                   WriteSavegame(gsets.gamemap, gsets.gamescale, gsets.lang);
                   RunAppCopy();  // Try restart the application. If unsuccessful, no problem, the settings are already saved
                   break;
              }
              DispatchMessageW(&msg);
          }
          
          Sleep(1); // Sleep, save the Battery!
          
          while(GetTickCount() > next_game_tick)
          {
              if ((SnakeLogic(&gsets.gamemap, &apple, &GameTicks, &anaconda, &bushmaster)) ||
                             (SnakeLogic(&gsets.gamemap, &apple, &GameTicks, &bushmaster, &anaconda)))
                 SnakeRestart(&gsets.gamemap, &anaconda, &bushmaster, &GameTicks);
              
              HDC sdc = GetDC(scores1);  // Draw scores 1
              ScoresShow(sdc, gsets.gamescale, anaconda.coins, anaconda.win, hFont, &ScoreTable, gsets.lang);
              ReleaseDC(scores1, sdc);
              
              HDC tdc = GetDC(scores2);  // Draw scores 2
              ScoresShow(tdc, gsets.gamescale, bushmaster.coins, bushmaster.win, hFont, &ScoreTable, gsets.lang);
              ReleaseDC(scores2, tdc);
              
              next_game_tick += GameTicks;
          }

          while(GetTickCount() > next_render_tick)
          {
              HDC dc = GetDC(game_map);  // Draw level and actors
              ActorsShow(dc, &gsets.gamemap, gsets.gamescale, anaconda.body, anaconda.len, bushmaster.body, bushmaster.len, &apple);
              ReleaseDC(game_map, dc);
              
              next_render_tick += RENDERLAG;
          }
     }
     return 0;
}