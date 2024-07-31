/* Simple GUI Snake game, written on pure C and WinAPI

   - logic.c        contains the code for all game logic
   - main.c         main and some auxiliary functions that pre-supply data for rendering
   - winproc.c      everything that directly renders the game and HUD
   - winio.c        processing keyboard input, reading and writing saves and translation files
   - snakestruct.h  structures for game logic to work
   - main.h         declarations for main()
   - winproc.h      structures for rendering
   - winio.h        predefined localization sizes and structure; required declarations

   For project build in MinGW64 with UTF-8 support, need use wchar_t types, w-functions,
   'L'-prefix for all strings and $gcc ... '-municode'

   2024 Ivan Tarasov ist76ist@gmail.com */

#include <stdio.h>
#include <windows.h>
#include "snakestruct.h"
#include "winproc.h"
#include "winio.h"
#include "main.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
    SaveData GSets = ReadSavegame();                             // Get previously saved data
    int GameTicks;                                               // Latency (ms) between game loops
    Snake *Anaconda   = malloc(sizeof(Snake));                   // First Snake
    Snake *Bushmaster = malloc(sizeof(Snake));                   // Second Snake
    Actors *AllActors = malloc(sizeof(Actors));                  // For render level, look at winproc.h
            AllActors->LevelWin.x = GSets.Map.x * GSets.Scale;   // Set game level size in px
            AllActors->LevelWin.y = GSets.Map.y * GSets.Scale;   // Heap is used instead of stack to keep stack less than 1MB
        Anaconda->Win = 0;
        Anaconda->Coins = 0;                                     // A crutch that prevents you from filling your MaxScore with garbage
        Anaconda->MaxScore = GSets.MaxS;
        Bushmaster->Win = 0;
    RECT ScoreTable;                                             // Size of score table
    SetRect(&ScoreTable, 0, 0, 7 * GSets.Scale,
           (GSets.Map.x/3 - 1) * GSets.Scale + GSets.Scale/2);
    GetGrid(AllActors, GSets.Map, GSets.Scale);                  // Array of points to draw the grid
    GetSnakeColors(AllActors, GSets.Mode);                       // Array of snakes cells colors
    GameLang Marks = ReadGamelang(GSets.Lang);                   // Get localization from file
    // WriteGameLang();  //  This function call generate Snake.lng file, if you need to create custom translate

    /* Create all GUI */
    WNDCLASSW wcl;
        memset(&wcl, 0, sizeof(WNDCLASSW));
        wcl.lpszClassName = L"mainwin";
        wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wcl.style = CS_GLOBALCLASS;
        wcl.lpfnWndProc = WndProc;
    RegisterClassW(&wcl);

    HWND hwnd = CreateWindowW(L"mainwin", L"Snake Party", 0x100A0000, 8, 8,
                             (AllActors->LevelWin.x + ScoreTable.right + (3 * GSets.Scale) / 2 + 2),
                             (AllActors->LevelWin.y + (5 * GSets.Scale) / 2 - 2), NULL, NULL, NULL, NULL);

    // Separate window with game level
    HWND GameMap = CreateWindowW(L"static", NULL, WS_VISIBLE | WS_CHILD , GSets.Scale/2 - 2,
                                GSets.Scale/2, AllActors->LevelWin.x, AllActors->LevelWin.y, hwnd, NULL, NULL, NULL);

    // Make Scoreboard 1 window
    HWND Scores1 = CreateWindowW(L"static", NULL, SS_CENTER | WS_VISIBLE | WS_CHILD , (GSets.Map.x+1)*GSets.Scale,
                                GSets.Scale/2, ScoreTable.right, ScoreTable.bottom, hwnd, NULL, NULL, NULL);

    // Make Scoreboard 2 window
    HWND Scores2 = CreateWindowW(L"static", NULL, SS_CENTER | WS_VISIBLE | WS_CHILD , (GSets.Map.x+1)*GSets.Scale,
                                ScoreTable.bottom + GSets.Scale + GSets.Scale/2,
                                ScoreTable.right, ScoreTable.bottom, hwnd, NULL, NULL, NULL);

    // Font for scoreboards
    HFONT hFont = CreateFontW(AllActors->LevelWin.y / 20, 0, 0, 0, 500, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                            OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, FALSE);

    // Small font for solutions
    HFONT hFontS = CreateFontW(AllActors->LevelWin.y / 34, 0, 0, 0, 500, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                            OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, FALSE);

    MakeMenu(hwnd, &GSets, &Marks);
    /* End of creating GUI */

    RandState = GetTickCount();                                       // For generate apple using custom func in logic.c
    Fruit Apple;                                                      // Make empty Apple struct
    SnakeRestart(&GSets, Anaconda, Bushmaster, &GameTicks, &Apple);   // Game first initialization
    DWORD NextGameTick = GetTickCount();                              // Timer for game loop
    DWORD NextRenderTick = GetTickCount();                            // Timer for render loop
    MSG   msg;                                                        // Messages from app
    HDC   dc;                                                         // Temporary context

    for (;(Anaconda->Len < 253) || (Bushmaster->Len < 253);) // Main Game loop. Remember, the Snake.Body[254]
    {
        if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // Processing keyboard commands
            if (msg.message == WM_KEYDOWN) DispatchVector(msg.wParam, &(Anaconda->NewVectr), &(Bushmaster->NewVectr),
                &NextGameTick, GSets.Mode);

            // When you select a menu item, the settings are overwritten and the application is restarted
            if (msg.message == WM_COMMAND)
            {
                DispatchMenu(msg.wParam, &GSets);
                WriteSavegame(&GSets, Anaconda->MaxScore);
                RunAppCopy();  // Try restart the application
                break;
            }

            // Autosave on exit
            if (msg.message == WM_QUIT)
            {
                WriteSavegame(&GSets, Anaconda->MaxScore);
                break;
            }
            DispatchMessageW(&msg);
        }

        Sleep(1); // Sleep, save the Battery!

        while (GetTickCount() > NextGameTick)
        {
            if (!GSets.Mode)  // if singleplayer
            {
                if (!SnakeLogic(&GSets, &Apple, &GameTicks, Anaconda, Bushmaster))  // Snake suicide?
                {
                    SnakeRestart(&GSets, Anaconda, Bushmaster, &GameTicks, &Apple);
                }
                dc = GetDC(Scores2);  // Draw solution in scores-2 window
                SolutionShow(dc, hFontS, &ScoreTable, Marks.str1503);
                ReleaseDC(Scores2, dc);
            }

            else
            {
                if (!SnakeLogic(&GSets, &Apple, &GameTicks, Anaconda, Bushmaster) ||
                    !SnakeLogic(&GSets, &Apple, &GameTicks, Bushmaster, Anaconda))
                {
                    SnakeRestart(&GSets, Anaconda, Bushmaster, &GameTicks, &Apple);
                }
                dc = GetDC(Scores2);  // Draw scores in scores-2 window
                ScoresShow(dc, Bushmaster, hFont, &ScoreTable, Marks.str1501, GSets.Mode);
                ReleaseDC(Scores2, dc);
            }

            // We calculate the coordinates of all Actors not every 16ms, but only ever GameTick
            SetApple(AllActors, &Apple, GSets.Scale);
            GetSnakesCells(AllActors, Anaconda, Bushmaster, &GSets);
            dc = GetDC(Scores1);  // Draw scores in scores-1 window
            ScoresShow(dc, Anaconda, hFont, &ScoreTable, GSets.Mode ?
                       Marks.str1501 : Marks.str1502, GSets.Mode);
            ReleaseDC(Scores1, dc);
            NextGameTick += GameTicks;
        }

        while (GetTickCount() > NextRenderTick)
        {
            dc = GetDC(GameMap);  // Draw level and actors
            ActorsShow(dc, AllActors, GSets.Mode);
            ReleaseDC(GameMap, dc);
            NextRenderTick += RENDERLAG;
        }
    }
    return 0;
}

// Make main menu
static inline void MakeMenu(HWND hwnd, SaveData *GSets, GameLang *Marks)
{

    HMENU MenuBar = CreateMenu();
    HMENU hPMenu1 = CreatePopupMenu();
    HMENU hPMenu2 = CreatePopupMenu();
    HMENU hPMenu3 = CreatePopupMenu();

    AppendMenuW(MenuBar, MF_STRING | MF_POPUP, (UINT_PTR)hPMenu1, Marks->str1008);
    AppendMenuW(MenuBar, MF_STRING | MF_POPUP, (UINT_PTR)hPMenu2, Marks->str1009);
    AppendMenuW(MenuBar, MF_STRING | MF_POPUP, (UINT_PTR)hPMenu3, Marks->str1110);
    AppendMenuW(MenuBar, MF_STRING,                         1100, Marks->str1100);
    AppendMenuW(hPMenu1, MF_STRING | (GSets->Map.x == SMALLMAPX  ? MF_CHECKED : MF_UNCHECKED), 1001, Marks->str1001);
    AppendMenuW(hPMenu1, MF_STRING | (GSets->Map.x == MEDIUMMAPX ? MF_CHECKED : MF_UNCHECKED), 1002, Marks->str1002);
    AppendMenuW(hPMenu1, MF_STRING | (GSets->Map.x == LARGEMAPX  ? MF_CHECKED : MF_UNCHECKED), 1003, Marks->str1003);
    AppendMenuW(hPMenu2, MF_STRING | (GSets->Scale == BIGSCALE   ? MF_CHECKED : MF_UNCHECKED), 1011, Marks->str1011);
    AppendMenuW(hPMenu2, MF_STRING | (GSets->Scale == HUGESCALE  ? MF_CHECKED : MF_UNCHECKED), 1012, Marks->str1012);
    AppendMenuW(hPMenu3, MF_STRING | (!GSets->Mode               ? MF_CHECKED : MF_UNCHECKED), 1013, Marks->str1013);
    AppendMenuW(hPMenu3, MF_STRING | (GSets->Mode                ? MF_CHECKED : MF_UNCHECKED), 1014, Marks->str1014);

    SetMenu(hwnd, MenuBar);
    SetMenu(hwnd, hPMenu1);
    SetMenu(hwnd, hPMenu2);
}

// Calculate Snake's coords before rendering
static inline void GetSnakesCells(Actors *Allobj, Snake const *Vyper, Snake const *Wutu, SaveData *const Sets)
{
    Allobj->ALen = Vyper->Len;
    for (short i = 0; i < Vyper->Len; i++)
    {
        Allobj->ASnake[i].left   = Vyper->Body[i].x * Sets->Scale;
        Allobj->ASnake[i].top    = Vyper->Body[i].y * Sets->Scale;
        Allobj->ASnake[i].right  = (Vyper->Body[i].x + 1) * Sets->Scale;
        Allobj->ASnake[i].bottom = (Vyper->Body[i].y + 1) * Sets->Scale;
    }
    if (!Sets->Mode) return;  // Single Player

    Allobj->BLen = Wutu->Len;
    for (short i = 0; i < Wutu->Len; i++)
    {
        Allobj->BSnake[i].left   = Wutu->Body[i].x * Sets->Scale + 1;
        Allobj->BSnake[i].top    = Wutu->Body[i].y * Sets->Scale + 1;
        Allobj->BSnake[i].right  = (Wutu->Body[i].x + 1) * Sets->Scale + 1;
        Allobj->BSnake[i].bottom = (Wutu->Body[i].y + 1) * Sets->Scale + 1;
    }
}

// Calculate Snake's cells color gradient (one time)
static void GetSnakeColors(Actors* Allobj, unsigned char Mode)
{
    for (short i = 0; i <= 63; i++)
    {
        Allobj->AColor[i] = Allobj->AColor[126 - i] = Allobj->AColor[126 + i] =
            Allobj->AColor[253 - i] = RGB(i * 4, 249, 255 - i * 4);
        if (Mode)
        {
            Allobj->BColor[i] = Allobj->BColor[126 - i] = Allobj->BColor[126 + i] =
                Allobj->BColor[253 - i] = RGB(191 + i, i * 4, 255 - i);
        }
    }
}

// Calculate grid lines only one time, use every 16ms
static void GetGrid(Actors *Allobj, CPoint Map, short Scale)
{
    short counter = 0;
    for (short i = 0; i < Map.x; i++)
    {
        Allobj->Grid[counter].left = (i + 1) * Scale;
        Allobj->Grid[counter].top = 1;
        Allobj->Grid[counter].right = (i + 1) * Scale;
        Allobj->Grid[counter].bottom = Map.y * Scale - 1;
        counter++;
    }
    for (short i = 0; i < Map.y; i++)
    {
        Allobj->Grid[counter].left = 1;
        Allobj->Grid[counter].top = (i + 1) * Scale;
        Allobj->Grid[counter].right = Map.x * Scale - 1;
        Allobj->Grid[counter].bottom = (i + 1) * Scale;
        counter++;
    }
    Allobj->GLen = counter;
}

// Calculate Apple coordinates & color
static inline void SetApple(Actors *Allobj, Fruit *Apple, short Scale)
{
    switch (Apple->Price)
    {
    case ColorGold:
        Allobj->AppleColor = 0x000080FF;
        break;

    case ColorBlack:
        Allobj->AppleColor = 0x00080808;
        break;

    default:
        Allobj->AppleColor = 0x000000FF;
        break;
    }
    Allobj->RApple.left = Apple->Coord.x * Scale + 2;
    Allobj->RApple.top = Apple->Coord.y * Scale + 2;
    Allobj->RApple.right = (Apple->Coord.x + 1) * Scale - 2;
    Allobj->RApple.bottom = (Apple->Coord.y + 1) * Scale - 2;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (message == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProc(hwnd, message, wparam, lparam);
}

/* An example of a function that creates a file with a localized interface.
Just write your lines in the appropriate positions and run the function
(See winio.h for details) */
/*
static void WriteGameLang(void)
{
    GameLang userlang = { .str1001 = L"Малый",        .str1002 = L"Средний",        .str1003 = L"Большой",
                            .str1008 = L"Размер карты", .str1009 = L"Масштаб",        .str1011 = L"Крупный",
                            .str1012 = L"Огромный",     .str1013 = L"Один игрок",     .str1014 = L"Два игрока",
                            .str1100 = L"Язык",         .str1110 = L"Режим игры",
                            .str1501 = L"\nОчков:\n\n%07i\n\nПобед:\n\n%i",
                            .str1502 = L"\nОчков:\n\n%07i\n\nРекорд\n\n%07i",
                            .str1503 = L"\nВыберите направление \nчтобы играть\n\nУправление: \nстрелки или\n«W S A D»\nна клавиатуре\n\nEscape - пауза\n\n\nИзменение любых\nнастроек требует\nперезапуска\n",
                        };
    HANDLE hFile = CreateFile(L"Snake.lng", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) return;
    WriteFile(hFile, &userlang, sizeof(userlang), NULL, NULL);
    CloseHandle(hFile);
}
*/