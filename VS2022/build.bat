@echo off
cl /nologo /W3 /O2 /fp:fast /Gm- /D_CRT_SECURE_NO_DEPRECATE /utf-8 /Fesnake2.exe main.c logic.c winio.c winproc.c user32.lib gdi32.lib /link /incremental:no
