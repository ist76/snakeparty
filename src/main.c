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
    SaveData GSets    = ReadSavegame();                          // Get previously saved data
    Snake *Anaconda   = malloc(sizeof(Snake));                   // First  Snake
    memset(Anaconda, 0, sizeof(Snake));
    Snake *Bushmaster = malloc(sizeof(Snake));                   // Second Snake
    memset(Bushmaster, 0, sizeof(Snake));
    Actors *AllActors = malloc(sizeof(Actors));                  // For render level, look at winproc.h
            AllActors->LevelWin.x = GSets.Map.x * GSets.Scale;   // Set game level size in px
            AllActors->LevelWin.y = GSets.Map.y * GSets.Scale;   // Heap is used instead of stack to keep stack less than 1MB
        Anaconda->MaxScore        = GSets.MaxS;
    RECT ScoreTable = { 0, 0, 7 * GSets.Scale,
                       (GSets.Map.x/3 - 1) * GSets.Scale
                        + GSets.Scale/2 };
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

    unsigned int State     = (unsigned int)GetTickCount();  // For generate apple using custom func in logic.c
    Fruit   Apple;                                          // Make empty Apple struct
    DWORD   NextGameTick   = GetTickCount();                // Timer for game loop
    DWORD   NextRenderTick = GetTickCount();                // Timer for render loop
    MSG     msg;                                            // Messages from app
    wchar_t Score[63];                                      // Messages for players
    int     GameTicks;                                      // Latency (ms) between game loops
    GetGrid(AllActors, GSets.Map, GSets.Scale);             // Array of points to draw the grid
    GetSnakeColors(AllActors, GSets.Mode);                  // Array of snakes cells colors
    SnakeRestart(&GSets, &Apple, &GameTicks,
                 Anaconda, Bushmaster, &State);             // Game logic first initialization
    DrawInterruption(GameMap, AllActors);

    for (;(Anaconda->Len < 253) || (Bushmaster->Len < 253);) // Main Game loop. Remember, the Snake.Body[254]
    {
        if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // Processing keyboard commands
            if (msg.message == WM_KEYDOWN) DispatchVector(msg.wParam, &(Anaconda->NewVectr),
                                           &(Bushmaster->NewVectr), &NextGameTick, GSets.Mode);

            // When you select a menu item, the settings are overwritten and the application is restarted
            if (msg.message == WM_COMMAND)
            {
                DispatchMenu  (msg.wParam, &GSets);
                WriteSavegame (&GSets, Anaconda->MaxScore);  // Autosave
                RunAppCopy();  // Try restart the application
                break;
            }
            if (msg.message == WM_QUIT)
            {
                WriteSavegame(&GSets, Anaconda->MaxScore);  // Autosave on exit
                break;
            }
            DispatchMessageW(&msg);
        }
        Sleep(INPUTLAG);      // Pause before new loop for reduce CPU usage

        if (GetTickCount() > NextGameTick)
        {
            if (!GSets.Mode)  // if singleplayer
            {
                if (!SnakeLogic(&GSets, &Apple, &GameTicks, Anaconda, Bushmaster, &State))  // Snake suicide?
                {
                    DrawInterruption(GameMap, AllActors);
                    SnakeRestart(&GSets, &Apple, &GameTicks, Anaconda, Bushmaster, &State);
                }
                SolutionShow  (Scores2, hFontS, &ScoreTable, Marks.str1503); // Draw solution in scores-2 window
            }
            else
            {
                if (!SnakeLogic (&GSets, &Apple, &GameTicks, Anaconda, Bushmaster, &State) ||
                    !SnakeLogic (&GSets, &Apple, &GameTicks, Bushmaster, Anaconda, &State))
                {
                    DrawInterruption(GameMap, AllActors);
                    SnakeRestart(&GSets, &Apple, &GameTicks, Anaconda, Bushmaster, &State);
                }
                SetInfo(Score, &Marks, Bushmaster, &GSets);
                SolutionShow  (Scores2, hFont, &ScoreTable, Score); // Draw scores in scores-2 window
            }
            // We calculate the coordinates of all Actors not every 16ms, but only ever GameTick
            SetApple       (AllActors, &Apple, GSets.Scale);
            GetSnakesCells (AllActors, Anaconda, Bushmaster, &GSets);
            SetInfo        (Score, &Marks, Anaconda, &GSets);
            SolutionShow   (Scores1, hFont, &ScoreTable, Score); // Draw scores in scores-1 window
            NextGameTick += GameTicks;
        }

        if (GetTickCount() > NextRenderTick)
        {
            // Draw level and actors
            ActorsShow     (GameMap, AllActors);
            NextRenderTick += RENDERLAG;
        }
    }
    return 0;
}

// Make main menu
static void MakeMenu(HWND hwnd, SaveData *GSets, GameLang *Marks)
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
static void GetSnakesCells(Actors *AllObj, Snake const *Vyper, Snake const *Wutu, SaveData *const Sets)
{
    AllObj->ALen = Vyper->Len;
    for (short i = 0; i < Vyper->Len; i++)
    {
        AllObj->ASnake[i].left   = Vyper->Body[i].x * Sets->Scale;
        AllObj->ASnake[i].top    = Vyper->Body[i].y * Sets->Scale;
        AllObj->ASnake[i].right  = (Vyper->Body[i].x + 1) * Sets->Scale;
        AllObj->ASnake[i].bottom = (Vyper->Body[i].y + 1) * Sets->Scale;
    }
    if (!Sets->Mode) return;  // Single Player

    AllObj->BLen = Wutu->Len;
    for (short i = 0; i < Wutu->Len; i++)
    {
        AllObj->BSnake[i].left   = Wutu->Body[i].x * Sets->Scale + 1;
        AllObj->BSnake[i].top    = Wutu->Body[i].y * Sets->Scale + 1;
        AllObj->BSnake[i].right  = (Wutu->Body[i].x + 1) * Sets->Scale + 1;
        AllObj->BSnake[i].bottom = (Wutu->Body[i].y + 1) * Sets->Scale + 1;
    }
}

// Calculate Snake's cells color gradient (one time)
static void GetSnakeColors(Actors* AllObj, unsigned char Mode)
{
    for (short i = 0; i <= 63; i++)
    {
        AllObj->AColor[i] = AllObj->AColor[126 - i]
                          = AllObj->AColor[126 + i]
                          = AllObj->AColor[253 - i]
                          = RGB(i * 4, 249, 255 - i * 4);
        if (!Mode) return;

        AllObj->BColor[i] = AllObj->BColor[126 - i]
                          = AllObj->BColor[126 + i]
                          = AllObj->BColor[253 - i]
                          = RGB(191 + i, i * 4, 255 - i);
    }
}

// Calculate grid lines only one time, use every 16ms
static void GetGrid(Actors *AllObj, CPoint Map, short Scale)
{
    short counter = 0;
    for (short i = 0; i < Map.x; i++)
    {
        AllObj->Grid[counter].left    = (i + 1) * Scale;
        AllObj->Grid[counter].top     = 1;
        AllObj->Grid[counter].right   = (i + 1) * Scale;
        AllObj->Grid[counter].bottom  = Map.y * Scale - 1;
        counter++;
    }
    for (short i = 0; i < Map.y; i++)
    {
        AllObj->Grid[counter].left    = 1;
        AllObj->Grid[counter].top     = (i + 1) * Scale;
        AllObj->Grid[counter].right   = Map.x * Scale - 1;
        AllObj->Grid[counter].bottom  = (i + 1) * Scale;
        counter++;
    }
    AllObj->GLen = counter;
}

// Calculate Apple coordinates & color
static void SetApple(Actors *AllObj, Fruit *Apple, short Scale)
{
    switch (Apple->Price)
    {
    case ColorGold:
        AllObj->AppleColor = 0x000080FF;
        break;

    case ColorBlack:
        AllObj->AppleColor = 0x00080808;
        break;

    default:
        AllObj->AppleColor = 0x000000FF;
        break;
    }
    AllObj->RApple.left   = Apple->Coord.x * Scale + 2;
    AllObj->RApple.top    = Apple->Coord.y * Scale + 2;
    AllObj->RApple.right  = (Apple->Coord.x + 1) * Scale - 2;
    AllObj->RApple.bottom = (Apple->Coord.y + 1) * Scale - 2;
}

// Generates text for game boards
static void SetInfo(wchar_t *Score, GameLang *Marks, Snake *Vyper, SaveData *Sets)
{
    Sets->Mode ? swprintf_s(Score, 63, Marks->str1501, Vyper->Coins, Vyper->Win):
                 swprintf_s(Score, 63, Marks->str1502, Vyper->Coins, Vyper->MaxScore);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (message == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProc(hwnd, message, wparam, lparam);
}

static void RunAppCopy(void)
{
     wchar_t path[256];
     GetModuleFileNameW(0, path, 256); // Get full name of snake.exe
     STARTUPINFOW si;  // Example of using CreateProcess from MSDN
     PROCESS_INFORMATION pi;
     memset( &si, 0, sizeof(si));
     si.cb = sizeof(si);
     memset(&pi, 0, sizeof(pi));
     CreateProcessW(path, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
     CloseHandle(pi.hProcess);
     CloseHandle(pi.hThread);
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