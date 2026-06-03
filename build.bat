@echo off
cl.exe -nologo -std:c11 -W4 -Zi -Od src\main.c -link -SUBSYSTEM:WINDOWS user32.lib gdi32.lib opengl32.lib -OUT:moottori_win.exe