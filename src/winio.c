/* Keyboard input, read and write settings, process menu commands, and restart the application */

#include <windows.h>
#include "snakestruct.h"
#include "winio.h"

// Converts keystrokes into movement direction (in game logic format)
void DispatchVector(WPARAM Key, CPoint * NewVect1, CPoint * NewVect2,
                    DWORD * NextTick, unsigned char Mode)
{
     switch (Key)
     {
     case 0x41:               // Key LEFT (A)
          NewVect1->x = -1;
          NewVect1->y =  0;
          break;

     case 0x44:               // Key RIGHT (D)
          NewVect1->x =  1;
          NewVect1->y =  0;
          break;

     case 0x57:               // Key UP (W)
          NewVect1->x =  0;
          NewVect1->y = -1;
          break;

     case 0x53:               // Key DOWN (S)
          NewVect1->x =  0;
          NewVect1->y =  1;
          break;

     case 0x25:               // Key LEFT (arrow)

          if (Mode)
          {
               NewVect2->x = -1;
               NewVect2->y =  0;
          }
          else
          {
               NewVect1->x = -1;
               NewVect1->y =  0;
          }
     break;

     case 0x27:               // Key RIGHT (arrow)

          if (Mode)
          {
               NewVect2->x =  1;
               NewVect2->y =  0;
          }
          else
          {
               NewVect1->x =  1;
               NewVect1->y =  0;
          }
          break;

     case 0x26:               // Key UP (arrow)

          if (Mode)
          {
               NewVect2->x =  0;
               NewVect2->y = -1;
          }
          else
          {
               NewVect1->x =  0;
               NewVect1->y = -1;
          }
          break;

     case 0x28:               // Key DOWN (arrow)

          if (Mode)
          {
               NewVect2->x =  0;
               NewVect2->y =  1;
          }
          else
          {
               NewVect1->x =  0;
               NewVect1->y =  1;
          }
          break;

     case 0x1B:               // Key PAUSE (ESC)
          *NextTick = *NextTick != UINT_MAX ? UINT_MAX : GetTickCount();
          break;

     default:
          break;
     }
}

// Handling menu item selections
void DispatchMenu(WPARAM Val, SaveData *GameSettings)
{
     switch (Val)
     {
     case 1001:
          GameSettings->Map.x = SMALLMAPX;
          GameSettings->Map.y = SMALLMAPY;
          break;

     case 1002:
          GameSettings->Map.x = MEDIUMMAPX;
          GameSettings->Map.y = MEDIUMMAPY;
          break;

     case 1003:
          GameSettings->Map.x = LARGEMAPX;
          GameSettings->Map.y = LARGEMAPY;
          break;

     case 1011:
          GameSettings->Scale = BIGSCALE;
          break;

     case 1012:
          GameSettings->Scale = HUGESCALE;
          break;

     case 1013:
          GameSettings->Mode = 0;
          break;

     case 1014:
          GameSettings->Mode = 1;
          break;

     case 1100:
          GameSettings->Lang = !(GameSettings->Lang);
          break;

     default:
          break;
     }
}

SaveData ReadSavegame(void) // No comments..
{
     SaveData usersave = { .Map = {.x = MEDIUMMAPX, .y = MEDIUMMAPY},
                           .Scale = BIGSCALE, .Lang = 0, .Mode = 0,
                           .MaxS = 0 };
     HANDLE hFile = CreateFileW(L"snake2.sav", GENERIC_READ, 0, NULL,
                                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
     if (INVALID_HANDLE_VALUE == hFile) return usersave; // FIXME! Write check correctness later
     ReadFile(hFile, &usersave, sizeof(usersave), NULL, NULL);
     CloseHandle(hFile);
     return usersave;
}

GameLang ReadGamelang(unsigned char Num)
{
     GameLang userlang = { .str1001 = L"Small",    .str1002 = L"Medium",        .str1003 = L"Large",
                           .str1008 = L"Map size", .str1009 = L"Scale",         .str1011 = L"Big",
                           .str1012 = L"Huge",     .str1013 = L"One Player", .str1014 = L"Two Players",
                           .str1100 = L"Language", .str1110 = L"Game Mode",
                           .str1501 = L"\nScore:\n\n%07i\n\nWins:\n\n%i",
                           .str1502 = L"\nScore\n\n%07i\n\nMax Score\n\n%07i",
                           .str1503 = L"\nTo start the game \njust select a direction\n\nSelecting direction - \narrows or «W S A D»\non the keyboard\n\nEscape - pause\n\n\nAll changes to game \nsettings require a \nrestart\n",
                         };
     if (Num)
     {
          HANDLE hFile = CreateFileW(L"snake.lng", GENERIC_READ, 0, NULL, OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL, NULL);
          if (INVALID_HANDLE_VALUE == hFile) return userlang; // FIXME! Write check correctness later
          ReadFile(hFile, &userlang, sizeof(userlang), NULL, NULL);
          CloseHandle(hFile);
     }
     return userlang;
}

void WriteSavegame(SaveData const *GameSettings, int MaxScore)
{
     HANDLE hFile = CreateFileW(L"snake2.sav", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL, NULL);
     if (INVALID_HANDLE_VALUE == hFile) return;
     SaveData usersave = { .Map = GameSettings->Map,
                           .Scale = GameSettings->Scale,
                           .Mode = GameSettings->Mode,
                           .Lang = GameSettings->Lang, .MaxS = MaxScore
                         };
     WriteFile(hFile, &usersave, sizeof(usersave), NULL, NULL);
     CloseHandle(hFile);
}