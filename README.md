# Tetris_s_max

NJUPT程序设计周-俄罗斯方块

## 快速开始

release下载后直接运行exe即可，背景/音乐/图标都是只读取文件夹下的第一个

## 编译

```bash
g++ main.c renderer.c game_logic.c input.c tetromino.c levels.c scene_level_select.c scene_story.c iob_fix.c -o Tetris_Su7_Max.exe -I./include -I"C:\msys64\mingw64\include" -L"C:\msys64\mingw64\lib" -leasyxw -lgdi32 -lole32 -luser32 -lwinmm -lcomdlg32 -lmsimg32 -DUNICODE -D_UNICODE -static -O2 -mwindows
```

不过路径得是自己电脑中的 `EasyX for MinGW` 移植后的

```bash
g++ main.c renderer.c game_logic.c input.c tetromino.c levels.c scene_level_select.c scene_story.c iob_fix.c -o tetris_s_max.exe -I./include -I"C:\msys64\mingw64\include" -L"C:\msys64\mingw64\lib" -leasyxw -lgdi32 -lole32 -luser32 -lwinmm -lcomdlg32 -lmsimg32 -DUNICODE -D_UNICODE
```

## 注意事项
打开exe之后有两个窗口，关闭游戏时记得都关闭！第一次打开时记得关闭音量
