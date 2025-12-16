#ifndef COMMON_H
#define COMMON_H

#include <graphics.h>
#include <time.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// --- 游戏常量 (Game Constants) ---

// 棋盘尺寸
#define BOARD_ROWS 20
#define BOARD_COLS 10

// 可变方块大小 (用于缩放)
extern int g_blockSize;

// 默认窗口尺寸 (作为缩放基准)
#define BASE_WINDOW_WIDTH 1000
#define BASE_WINDOW_HEIGHT 700

// 动态窗口变量
extern int g_windowWidth;
extern int g_windowHeight;

// 当前播放的音乐标题 (用于滚动显示)
extern TCHAR g_currentMusicTitle[256];

// 双人模式时间限制 (秒)，-1 表示不限时
extern int g_dualTimeLimit;

// 游戏机制
#define LINES_PER_LEVEL 10
#define BASE_SCORE_PER_LINE 100
#define MAX_LEVEL 10
// DUAL_MODE_TIME_LIMIT 宏已移除，改为 g_dualTimeLimit 变量

// 颜色定义
#define COLOR_GRID LIGHTGRAY
#define COLOR_BORDER WHITE
#define COLOR_TEXT YELLOW
#define COLOR_BUTTON_NORMAL RGB(50, 50, 50)
#define COLOR_BUTTON_HOVER RGB(80, 80, 80)
#define COLOR_BUTTON_TEXT WHITE

// --- 数据结构 ---

typedef enum {
    STATE_INTRO,        // 开场动画
    STATE_MENU,         // 主菜单
    STATE_SETTINGS,     // 设置
    STATE_LEADERBOARD,  // 排行榜
    STATE_TIME_SELECT,  // 双人模式时间选择
    STATE_GAME_SINGLE,  // 单人游戏
    STATE_GAME_DUAL,    // 双人对战
    STATE_GAMEOVER      // 游戏结束
} GameState;

typedef enum {
    MODE_SINGLE, // 单人模式
    MODE_DUAL    // 双人模式
} GameMode;

// 设置相关
typedef enum {
    DIFF_EASY,   // 简单
    DIFF_NORMAL, // 普通
    DIFF_HARD    // 困难
} Difficulty;

#define MAX_PATH_LEN 260

typedef struct {
    bool musicEnabled;          // 音乐开关
    Difficulty difficulty;      // 难度
    
    TCHAR musicPath[MAX_PATH_LEN];   // 音乐文件路径
    TCHAR bgImagePath[MAX_PATH_LEN]; // 背景图片路径
    int bgOpacity;                   // 背景不透明度 (0-100)
    int musicVolume;                 // 音乐音量 (0-1000)
    
    // 窗口大小缓存
    int windowWidth;
    int windowHeight;
} GameSettings;

extern GameSettings g_settings;

// 排行榜
#define MAX_LEADERBOARD_ENTRIES 5

typedef struct {
    char name[32]; // 简单的 ASCII 名字
    int score;
    time_t date;
} LeaderboardEntry;

typedef struct {
    LeaderboardEntry entries[MAX_LEADERBOARD_ENTRIES];
    int count;
} Leaderboard;

extern Leaderboard g_leaderboard;

// 方块类型
typedef enum {
    BLOCK_I = 0,
    BLOCK_O,
    BLOCK_T,
    BLOCK_L,
    BLOCK_J,
    BLOCK_S,
    BLOCK_Z,
    BLOCK_TYPE_COUNT
} BlockType;

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    BlockType type;
    int rotation;
    int x;
    int y;
    Point shape[4];
    COLORREF color;
} Block;

// 按键状态结构体 (用于 DAS/ARR)
typedef struct {
    bool isHeld;
    clock_t firstPressTime;
    clock_t lastRepeatTime;
} KeyState;

typedef struct {
    int id;
    int board[BOARD_ROWS][BOARD_COLS];
    COLORREF boardColors[BOARD_ROWS][BOARD_COLS];
    Block currentBlock;
    Block nextBlock;
    int score;
    int level;
    int linesCleared;
    bool isGameOver;
    bool isWinner;
    clock_t lastDropTime;
    int dropInterval;
    bool isPaused; // 暂停状态
    
    // 按键状态跟踪
    KeyState keyLeft;
    KeyState keyRight;
    KeyState keyDown;
    KeyState keyDrop; // 用于双击检测
} PlayerState;

// 简单的按钮结构
typedef struct {
    int x, y, w, h;
    const TCHAR* text;
} Button;

// 设置加载与保存
void load_settings();
void save_settings();

// 快速下落
void hard_drop(PlayerState *p);

#endif // COMMON_H
