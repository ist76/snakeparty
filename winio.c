// Keyboard input, read and write settings, process menu commands,
// and restart the application

#include <windows.h>
#include "snakestruct.h"
#include "winio.h"

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
          *lang = !(*lang);
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