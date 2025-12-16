# Tetris_s_max

NJUPT程序设计周-俄罗斯方块

## 快速开始

release下载后直接运行exe即可

## 编译

```bash
g++ -static main.c renderer.c game_logic.c input.c tetromino.c iob_fix.c resource.o -o tetris_s_max.exe -I./include -I"C:\msys64\mingw64\include" -L"C:\msys64\mingw64\lib" -leasyxw -lgdi32 -lole32 -luser32 -lwinmm -lcomdlg32 -lmsimg32 -DUNICODE -D_UNICODE
```

不过路径得是自己电脑中的 `EasyX for MinGW` 移植后的

```bash
g++ main.c renderer.c game_logic.c input.c tetromino.c iob_fix.c resource.o -o tetris.exe -I./include -I"C:\msys64\mingw64\include" -L"C:\msys64\mingw64\lib" -leasyxw -lgdi32 -lole32 -luser32 -lwinmm -lcomdlg32 -lmsimg32 -DUNICODE -D_UNICODE
```