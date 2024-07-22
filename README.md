# Snake Party

_Snake game for two players. Written in C and WinAPI, compiled in the MinGW64 environment_

![Snake Party](https://github.com/ist76/snakeparty/blob/main/Screenshot.PNG?raw=true)

## About
Variation of the game "Snake" for one or two players. Development and expansion [Lonely Snake](https://github.com/ist76/lonelysnake).
- Very small size (~25kb without icon) of the executable file
- Simple and obvious (hopefully) source code
- MSVS was not used either during writing or during assembly, the entire project can be assembled with one command in the console
- Two interface languages ​​to choose from. The translation is stored as a separate file in the game directory, so localization into other languages ​​will be very simple.
- WinAPI is used to display the game. All the functions are old, I suspect that the game will be built without edits or with minimal edits even for 32-bit Windows XP. On the other hand, OSes other than the Windows family are absolutely not supported (for obvious reasons). However, the game logic is separated from the interface and nothing prevents you from writing a new interface.

## Download and Installation
The "bin" folder already contains an executable file compiled for 64-bit systems and sample translation file. You just need to save them in a convenient place and run snake2.exe. The game creates a settings file in its working directory. Removing it resets the settings to default.
If you want to build the game from the source yourself, the makefile is attached. I compiled the application in MinGW-w64 && MSYS2 environment with gcc and with clang. I guess there shouldn't be any problems.

## Game
Controls for the first player are the "W-S-A-D" keys on the keyboard. For the second - cursor keys. Points are awarded for “eaten” apples. Spent on movement. The player whose points run out loses. When the “snake” goes beyond the edges of the field, it appears on the opposite edge of the map. You can't bite yourself, it's a loss. You can pass through your opponent, this has no consequences.

## License
Of course, you can use all or any part of the code provided (do you really want to?). I'm not sure about the specific license version under which the code could be published (due to -static -lgdi32). There's no point in thinking about it yet.
