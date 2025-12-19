@echo off
echo Compiling Portable Release Version...
g++ main.c renderer.c game_logic.c input.c tetromino.c levels.c scene_level_select.c scene_story.c iob_fix.c -o Tetris_Su7_Max.exe -I./include -I"C:\msys64\mingw64\include" -L"C:\msys64\mingw64\lib" -leasyxw -lgdi32 -lole32 -luser32 -lwinmm -lcomdlg32 -lmsimg32 -DUNICODE -D_UNICODE -static -O2 -mwindows
if %errorlevel% neq 0 (
    echo Compilation failed!
    pause
    exit /b %errorlevel%
)
echo ---------------------------------------
echo Compilation Successful!
echo Generated: Tetris_Su7_Max.exe
echo zip with images/musics/icons
pause