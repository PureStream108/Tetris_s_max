@echo off
echo Compiling Tetris...
g++ main.c renderer.c game_logic.c input.c tetromino.c levels.c scene_level_select.c scene_story.c iob_fix.c -o tetris_s_max.exe -I./include -I"C:\msys64\mingw64\include" -L"C:\msys64\mingw64\lib" -leasyxw -lgdi32 -lole32 -luser32 -lwinmm -lcomdlg32 -lmsimg32 -DUNICODE -D_UNICODE
if %errorlevel% neq 0 (
    echo Compilation failed!
    pause
    exit /b %errorlevel%
)
echo Compilation successful! Run .exe to play.
pause
