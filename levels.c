#include "include/levels.h"
#include <stdio.h>
#include <stdlib.h>

LevelData g_levels[LEVEL_COUNT];

void init_levels() {
    // 初始化默认数据
    for (int i = 0; i < LEVEL_COUNT; i++) {
        g_levels[i].id = (LevelID)i;
        g_levels[i].isUnlocked = false;
        g_levels[i].isCompleted = false;
        g_levels[i].restrictPieces = false;
        g_levels[i].garbageType = GARBAGE_RANDOM;
    }

    // --- PM-1 ---
    _stprintf(g_levels[LEVEL_1].name, _T("PM-1"));
    _stprintf(g_levels[LEVEL_1].title, _T("PM-1 天穹市"));
    _stprintf(g_levels[LEVEL_1].story, _T("引言：天穹市的起源？不不不，根本没有任何资料从起源那一代人那流传下来，故本书只从最早被发现的资料开始记起(500年前)。\n---《天穹日志》\n\n"));
    g_levels[LEVEL_1].targetLines = 10;
    g_levels[LEVEL_1].initialGarbageRows = 4;
    g_levels[LEVEL_1].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_1].mapX = 100;
    g_levels[LEVEL_1].mapY = 400;
    g_levels[LEVEL_1].isUnlocked = true; // 第一关默认解锁

    // --- PM-2 ---
    _stprintf(g_levels[LEVEL_2].name, _T("PM-2"));
    _stprintf(g_levels[LEVEL_2].title, _T("PM-2 镜面"));
    _stprintf(g_levels[LEVEL_2].story, _T("你已经熟悉了操作。\n但挑战才刚刚开始。\n货物堆积得越来越高了。\n\n任务：消除17行。\n底部的混乱在增加。"));
    g_levels[LEVEL_2].targetLines = 17;
    g_levels[LEVEL_2].initialGarbageRows = 5;
    g_levels[LEVEL_2].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_2].mapX = 300;
    g_levels[LEVEL_2].mapY = 300;

    // --- PM-3 ---
    _stprintf(g_levels[LEVEL_3].name, _T("PM-3"));
    _stprintf(g_levels[LEVEL_3].title, _T("PM-3 天穹咖啡馆"));
    _stprintf(g_levels[LEVEL_3].story, _T("随着深入，能见度在降低。\n（其实并没有，只是剧情需要）\n你需要更快的反应。\n\n任务：消除24行。\n垃圾行增加到了6行。"));
    g_levels[LEVEL_3].targetLines = 24;
    g_levels[LEVEL_3].initialGarbageRows = 6;
    g_levels[LEVEL_3].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_3].mapX = 500;
    g_levels[LEVEL_3].mapY = 400;

    // --- PM-4 ---
    _stprintf(g_levels[LEVEL_4].name, _T("PM-4"));
    _stprintf(g_levels[LEVEL_4].title, _T("PM-4 计划"));
    _stprintf(g_levels[LEVEL_4].story, _T("不要慌张，稳扎稳打。\n每一个方块的放置都至关重要。\n\n任务：消除31行。\n初始垃圾行：7行。"));
    g_levels[LEVEL_4].targetLines = 31;
    g_levels[LEVEL_4].initialGarbageRows = 7;
    g_levels[LEVEL_4].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_4].mapX = 700;
    g_levels[LEVEL_4].mapY = 300;

    // --- PM-5 ---
    _stprintf(g_levels[LEVEL_5].name, _T("PM-5"));
    _stprintf(g_levels[LEVEL_5].title, _T("PM-5 半途而废？"));
    _stprintf(g_levels[LEVEL_5].story, _T("很多人倒在了这里。你会是例外吗？"));
    g_levels[LEVEL_5].targetLines = 38;
    g_levels[LEVEL_5].initialGarbageRows = 8;
    g_levels[LEVEL_5].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_5].mapX = 900;
    g_levels[LEVEL_5].mapY = 400;

    // --- PM-6 ---
    _stprintf(g_levels[LEVEL_6].name, _T("PM-6"));
    _stprintf(g_levels[LEVEL_6].title, _T("PM-6 逆流而上"));
    _stprintf(g_levels[LEVEL_6].story, _T("压力越来越大。"));
    g_levels[LEVEL_6].targetLines = 45;
    g_levels[LEVEL_6].initialGarbageRows = 9;
    g_levels[LEVEL_6].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_6].mapX = 1100;
    g_levels[LEVEL_6].mapY = 300;

    // --- PM-7 ---
    _stprintf(g_levels[LEVEL_7].name, _T("PM-7"));
    _stprintf(g_levels[LEVEL_7].title, _T("PM-7 黎明之前"));
    _stprintf(g_levels[LEVEL_7].story, _T("黑暗最浓的时刻。你需要寻找那一线生机。"));
    g_levels[LEVEL_7].targetLines = 52;
    g_levels[LEVEL_7].initialGarbageRows = 10;
    g_levels[LEVEL_7].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_7].mapX = 1300;
    g_levels[LEVEL_7].mapY = 400;

    // --- PM-8 ---
    _stprintf(g_levels[LEVEL_8].name, _T("PM-8"));
    _stprintf(g_levels[LEVEL_8].title, _T("PM-8 绝境求生"));
    _stprintf(g_levels[LEVEL_8].story, _T("没有退路了。\n只能不断向下消除。"));
    g_levels[LEVEL_8].targetLines = 59;
    g_levels[LEVEL_8].initialGarbageRows = 11;
    g_levels[LEVEL_8].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_8].mapX = 1500;
    g_levels[LEVEL_8].mapY = 300;

    // --- PM-9 ---
    _stprintf(g_levels[LEVEL_9].name, _T("PM-9"));
    _stprintf(g_levels[LEVEL_9].title, _T("PM-9 妥协"));
    _stprintf(g_levels[LEVEL_9].story, _T("只差最后一步。\n你能听到胜利的呼唤吗？\n还是说，这只是幻觉？"));
    g_levels[LEVEL_9].targetLines = 66;
    g_levels[LEVEL_9].initialGarbageRows = 12;
    g_levels[LEVEL_9].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_9].mapX = 1700;
    g_levels[LEVEL_9].mapY = 400;

    // --- PM-10 ---
    _stprintf(g_levels[LEVEL_10].name, _T("PM-10"));
    _stprintf(g_levels[LEVEL_10].title, _T("PM-10 天空井"));
    _stprintf(g_levels[LEVEL_10].story, _T("人们小时候就在想，只有天穹塔能高于虚假之天，那上面的人们，能看到什么？\n----《天穹日志》\n\n从天穹塔向下望的那一刻才明白。所谓天穹市，不过就是一个巨大的井。\n人们策划以年为单位的计划，只为去外面看看，现在在你看来，也不过是个笑话"));
    g_levels[LEVEL_10].targetLines = 70;
    g_levels[LEVEL_10].initialGarbageRows = 14;
    g_levels[LEVEL_10].garbageType = GARBAGE_RANDOM;
    g_levels[LEVEL_10].mapX = 1900;
    g_levels[LEVEL_10].mapY = 300;

    // --- PM-EX-1 ---
    _stprintf(g_levels[LEVEL_EX_1].name, _T("PM-EX-1"));
    _stprintf(g_levels[LEVEL_EX_1].title, _T("PM-EX-1 因为山就在那儿"));
    _stprintf(g_levels[LEVEL_EX_1].story, _T("xxx"));
    g_levels[LEVEL_EX_1].targetLines = 40;
    g_levels[LEVEL_EX_1].initialGarbageRows = 14;
    g_levels[LEVEL_EX_1].garbageType = GARBAGE_CONVEX;
    g_levels[LEVEL_EX_1].mapX = 2100;
    g_levels[LEVEL_EX_1].mapY = 200;

    // --- PM-EX-2 ---
    _stprintf(g_levels[LEVEL_EX_2].name, _T("PM-EX-2"));
    _stprintf(g_levels[LEVEL_EX_2].title, _T("PM-EX-2 坐天观井"));
    _stprintf(g_levels[LEVEL_EX_2].story, _T("xxx"));
    g_levels[LEVEL_EX_2].targetLines = 50;
    g_levels[LEVEL_EX_2].initialGarbageRows = 14;
    g_levels[LEVEL_EX_2].garbageType = GARBAGE_CONCAVE;
    g_levels[LEVEL_EX_2].mapX = 2300;
    g_levels[LEVEL_EX_2].mapY = 500;

    // --- PM-EX-3 ---
    _stprintf(g_levels[LEVEL_EX_3].name, _T("PM-EX-3"));
    _stprintf(g_levels[LEVEL_EX_3].title, _T("PM-EX-3 下一站"));
    _stprintf(g_levels[LEVEL_EX_3].story, _T("xxx"));
    g_levels[LEVEL_EX_3].targetLines = 30;
    g_levels[LEVEL_EX_3].initialGarbageRows = 0; // 无初始垃圾
    g_levels[LEVEL_EX_3].restrictPieces = true;
    g_levels[LEVEL_EX_3].mapX = 2500;
    g_levels[LEVEL_EX_3].mapY = 350;
}

void generate_level_garbage(PlayerState *p, LevelID id) {
    LevelData *level = &g_levels[id];
    int rows = level->initialGarbageRows;
    if (rows <= 0) return;

    // 确保不超出棋盘
    if (rows >= BOARD_ROWS) rows = BOARD_ROWS - 1;

    // 从底部开始填充
    int startRow = BOARD_ROWS - rows;

    for (int r = startRow; r < BOARD_ROWS; r++) {
        // 根据类型生成行
        if (level->garbageType == GARBAGE_RANDOM) {
            // 随机生成一行，留1-2个空位
            for (int c = 0; c < BOARD_COLS; c++) {
                if (rand() % 10 < 8) { // 80% 概率有方块
                     p->board[r][c] = 1; // 简单的灰色块
                     p->boardColors[r][c] = RGB(100, 100, 100);
                } else {
                     p->board[r][c] = 0;
                     p->boardColors[r][c] = BLACK;
                }
            }
            // 确保至少有一个空位，防止一开始就消除了
            int hole = rand() % BOARD_COLS;
            p->board[r][hole] = 0;
            p->boardColors[r][hole] = BLACK;

        } else if (level->garbageType == GARBAGE_CONVEX) {
            // 凸型：中间高，两边低
            // 计算当前行相对于底部的距离 (0是底部)
            int distFromBottom = BOARD_ROWS - 1 - r;
            // 越高越窄
            // 底部宽度 10, 每向上几行缩减宽度
            int width = 10 - distFromBottom * 2; 
            if (width < 2) width = 2;
            
            int startCol = (BOARD_COLS - width) / 2;
            int endCol = startCol + width;
            
            for (int c = 0; c < BOARD_COLS; c++) {
                if (c >= startCol && c < endCol) {
                    p->board[r][c] = 1;
                    p->boardColors[r][c] = RGB(100, 100, 100);
                } else {
                    p->board[r][c] = 0;
                    p->boardColors[r][c] = BLACK;
                }
            }

        } else if (level->garbageType == GARBAGE_CONCAVE) {
            // 凹型：两边高，中间低
            int distFromBottom = BOARD_ROWS - 1 - r;
            // 越高中间空隙越大
            int gap = distFromBottom * 2;
            if (gap > 8) gap = 8;
            
            int startGap = (BOARD_COLS - gap) / 2;
            int endGap = startGap + gap;
            
            for (int c = 0; c < BOARD_COLS; c++) {
                if (c < startGap || c >= endGap) {
                    p->board[r][c] = 1;
                    p->boardColors[r][c] = RGB(100, 100, 100);
                } else {
                    p->board[r][c] = 0;
                    p->boardColors[r][c] = BLACK;
                }
            }
        }
    }
}

LevelData* get_level_data(LevelID id) {
    if (id < 0 || id >= LEVEL_COUNT) return NULL;
    return &g_levels[id];
}

bool is_level_completed(LevelID id) {
    if (id < 0 || id >= LEVEL_COUNT) return false;
    return g_levels[id].isCompleted;
}

void set_level_completed(LevelID id, bool completed) {
    if (id < 0 || id >= LEVEL_COUNT) return;
    g_levels[id].isCompleted = completed;
}

void unlock_next_level(LevelID currentId) {
    if (currentId + 1 < LEVEL_COUNT) {
        g_levels[currentId + 1].isUnlocked = true;
        // 更新最大解锁关卡索引（用于存档）
        if ((int)currentId + 1 > g_settings.maxUnlockedLevel) {
            g_settings.maxUnlockedLevel = (int)currentId + 1;
        }
        // 同时更新剧情解锁进度
        if ((int)currentId + 1 > g_settings.maxUnlockedStoryLevel) {
            g_settings.maxUnlockedStoryLevel = (int)currentId + 1;
        }
        save_settings();
    }
}
