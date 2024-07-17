# Snake Party

_Snake game for two players. Written in C and WinAPI, compiled in the MinGW64 environment_

## About
Variation of the game "Snake" for two players. Further development [Lonely Snake](https://github.com/ist76/lonelysnake).
- Very small size (~20kb without icon) of the executable file
- Simple and obvious (hopefully) source code with comments
- MSVS was not used either during writing or during assembly, the entire project can be assembled with one command in the console
- Two interface languages ​​to choose from. Only two, it’s hardcoded, but by editing the source code, a version for any language should be done in a couple of minutes.
- WinAPI is used to display the game. All the functions are old, I suspect that the game will be built without edits or with minimal edits even for 32-bit Windows XP. On the other hand, OSes other than the Windows family are absolutely not supported (for obvious reasons). However, the game logic is separated from the interface and nothing prevents you from writing a new interface.

## Download and Installation
The "bin" folder already contains an executable file compiled for 64-bit systems. You only need him. The game creates a settings file in its working directory. Removing it resets the settings to default. Place it in any place convenient for you.
If you want to build the game from the source yourself, the makefile is attached. I compiled the application in MinGW-w64 && MSYS2 environment. I guess there shouldn't be any problems.

## License
Of course, you can use all or any part of the code provided (do you really want to?). I'm not sure about the specific license version under which the code could be published (due to -static -lgdi32). There's no point in thinking about it yet.
