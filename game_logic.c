#include "include/game_logic.h"
#include "include/tetromino.h"
#include "include/levels.h" // 引入关卡相关定义

// 前向声明
void lock_block(PlayerState *p);
void clear_lines(PlayerState *p);
void spawn_next_block(PlayerState *p);

Leaderboard g_leaderboard = {0};

extern GameState g_gameState; // 需要访问全局游戏状态

// 辅助函数：根据等级和难度更新速度
void update_speed(PlayerState *p) {
    int baseSpeed = 1000;
    int decrement = 100;
    
    if (g_settings.difficulty == DIFF_EASY) {
        baseSpeed = 1200;
        decrement = 80;
    } else if (g_settings.difficulty == DIFF_HARD) {
        baseSpeed = 800;
        decrement = 120;
    }

    int newInterval = baseSpeed - (p->level - 1) * decrement;
    if (newInterval < 50) newInterval = 50;
    p->dropInterval = newInterval;
}

void init_player(PlayerState *p, int id) {
    p->id = id;
    p->score = 0;
    p->level = 1;
    p->linesCleared = 0;
    p->isGameOver = false;
    p->isWinner = false;
    p->isPaused = false;
    p->restrictPieces = false; // 默认不限制
    p->levelTargetLines = 9999; // 默认无限
    p->levelStartTime = clock();
    
    // 设置初始下落间隔
    update_speed(p);
    
    p->lastDropTime = clock();
    
    // 清空棋盘
    for (int r = 0; r < BOARD_ROWS; r++) {
        for (int c = 0; c < BOARD_COLS; c++) {
            p->board[r][c] = 0;
            p->boardColors[r][c] = BLACK;
        }
    }
    
    // 生成初始方块 (如果 restrictPieces 在此之后被修改，调用者需要重新生成)
    spawn_block(&p->currentBlock, p->restrictPieces);
    spawn_block(&p->nextBlock, p->restrictPieces);
}

void start_level(PlayerState *p, int playerId, int levelId) {
    // 1. 基础初始化
    init_player(p, playerId);
    
    // 2. 应用关卡配置
    p->currentLevelIndex = levelId;
    p->levelTargetLines = g_levels[levelId].targetLines;
    p->restrictPieces = g_levels[levelId].restrictPieces;
    
    // 3. 如果有限制条件，重新生成初始方块
    // (因为 init_player 默认用 restrictPieces=false 生成了一次)
    if (p->restrictPieces) {
        spawn_block(&p->currentBlock, true);
        spawn_block(&p->nextBlock, true);
    }
    
    // 4. 生成关卡地形
    generate_level_garbage(p, (LevelID)levelId);
}

// 检查特定方块状态的碰撞
bool check_collision(PlayerState *p, Block *b) {
    for (int i = 0; i < 4; i++) {
        int bx = b->x + b->shape[i].x;
        int by = b->y + b->shape[i].y;
        
        // 边界检查
        if (bx < 0 || bx >= BOARD_COLS || by >= BOARD_ROWS) return true;
        
        // 占用检查 (忽略 by < 0，即在棋盘上方的情况)
        if (by >= 0 && p->board[by][bx] != 0) return true;
    }
    return false;
}

bool move_block(PlayerState *p, int dx, int dy) {
    if (p->isGameOver || p->isPaused) return false;
    
    Block temp = p->currentBlock;
    temp.x += dx;
    temp.y += dy;
    
    if (!check_collision(p, &temp)) {
        p->currentBlock = temp;
        return true;
    }
    return false;
}

// 快速下落：直接移动到底部并锁定
void hard_drop(PlayerState *p) {
    if (p->isGameOver || p->isPaused) return;
    
    // 循环向下移动直到无法移动
    while (move_block(p, 0, 1)) {
        // 继续移动
    }
    
    // 落地锁定
    lock_block(p);
    clear_lines(p);
    spawn_next_block(p);
    
    // 重置下落计时器，避免立即再次触发自动下落
    p->lastDropTime = clock();
}

void rotate_block_action(PlayerState *p) {
    if (p->isGameOver || p->isPaused) return;
    
    Block temp = p->currentBlock;
    rotate_block_data(&temp);
    
    if (!check_collision(p, &temp)) {
        p->currentBlock = temp;
    }
}

void lock_block(PlayerState *p) {
    Block *b = &p->currentBlock;
    for (int i = 0; i < 4; i++) {
        int bx = b->x + b->shape[i].x;
        int by = b->y + b->shape[i].y;
        
        if (by >= 0 && by < BOARD_ROWS && bx >= 0 && bx < BOARD_COLS) {
            p->board[by][bx] = 1; // 标记占用
            p->boardColors[by][bx] = b->color;
        }
    }
}

void clear_lines(PlayerState *p) {
    int lines = 0;
    
    for (int r = BOARD_ROWS - 1; r >= 0; r--) {
        bool full = true;
        for (int c = 0; c < BOARD_COLS; c++) {
            if (p->board[r][c] == 0) {
                full = false;
                break;
            }
        }
        
        if (full) {
            lines++;
            // 下移上方行
            for (int nr = r; nr > 0; nr--) {
                for (int c = 0; c < BOARD_COLS; c++) {
                    p->board[nr][c] = p->board[nr - 1][c];
                    p->boardColors[nr][c] = p->boardColors[nr - 1][c];
                }
            }
            // 清空顶行
            for (int c = 0; c < BOARD_COLS; c++) {
                p->board[0][c] = 0;
                p->boardColors[0][c] = BLACK;
            }
            r++; // 重新检查这一行 (因为它现在是上面掉下来的那一行)
        }
    }
    
    if (lines > 0) {
        p->linesCleared += lines;
        
        // 积分规则优化：
        // 1 行: 100分
        // 2~4 行: (行数 + 1) * 100 分
        if (lines == 1) {
            p->score += 100;
        } else {
            p->score += (lines + 1) * 100;
        }
        
        // 关卡模式：检查是否达成目标
        if (g_gameState == STATE_GAME_LEVEL) {
            if (p->linesCleared >= p->levelTargetLines) {
                p->isWinner = true;
                g_gameState = STATE_GAME_LEVEL_WIN;
                
                // 记录完成并解锁下一关
                set_level_completed((LevelID)p->currentLevelIndex, true);
                unlock_next_level((LevelID)p->currentLevelIndex);
            }
            // 关卡模式下不自动增加难度等级 (或者可以增加，取决于设计，这里暂时不增加以免过于复杂)
            // 如果希望随着行数增加速度，可以保留下面的逻辑
        } else {
            // 普通模式：每10行升级
            int newLevel = 1 + p->linesCleared / LINES_PER_LEVEL;
            if (newLevel > MAX_LEVEL) newLevel = MAX_LEVEL;
            
            if (newLevel > p->level) {
                p->level = newLevel;
                update_speed(p);
            }
        }
    }
}

void spawn_next_block(PlayerState *p) {
    p->currentBlock = p->nextBlock;
    // 使用 restrictPieces 参数
    spawn_block(&p->nextBlock, p->restrictPieces);
    
    // 检查新生成的方块是否立即碰撞 -> 游戏结束
    if (check_collision(p, &p->currentBlock)) {
        p->isGameOver = true;
    }
}

void update_game(PlayerState *p) {
    if (p->isGameOver || p->isPaused || p->isWinner) {
        // 更新 lastDropTime 以免暂停恢复后立即下落
        p->lastDropTime = clock();
        return;
    }
    
    clock_t now = clock();
    if (now - p->lastDropTime > p->dropInterval) {
        // 尝试下落
        if (!move_block(p, 0, 1)) {
            // 落地
            lock_block(p);
            clear_lines(p);
            // 只有当游戏未结束且未胜利时才生成下一个
            if (!p->isGameOver && !p->isWinner) {
                spawn_next_block(p);
            }
        }
        p->lastDropTime = now;
    }
}

bool check_game_over(PlayerState *p) {
    return p->isGameOver;
}

// --- 排行榜实现 ---

void load_leaderboard() {
    FILE* f = fopen("leaderboard.dat", "rb");
    if (f) {
        fread(&g_leaderboard, sizeof(Leaderboard), 1, f);
        fclose(f);
    } else {
        g_leaderboard.count = 0;
    }
}

void save_leaderboard() {
    FILE* f = fopen("leaderboard.dat", "wb");
    if (f) {
        fwrite(&g_leaderboard, sizeof(Leaderboard), 1, f);
        fclose(f);
    }
}

// qsort 比较函数 (降序)
int compare_entries(const void* a, const void* b) {
    return ((LeaderboardEntry*)b)->score - ((LeaderboardEntry*)a)->score;
}

void add_to_leaderboard(int score) {
    // 如果已满且分数低于最低分，忽略
    if (g_leaderboard.count == MAX_LEADERBOARD_ENTRIES && score <= g_leaderboard.entries[MAX_LEADERBOARD_ENTRIES-1].score) {
        return;
    }
    
    // 添加条目
    int idx = g_leaderboard.count;
    if (idx < MAX_LEADERBOARD_ENTRIES) {
        g_leaderboard.count++;
    } else {
        idx = MAX_LEADERBOARD_ENTRIES - 1; // 覆盖最后一个
    }
    
    // 简单的默认名字
    strcpy(g_leaderboard.entries[idx].name, "Player");
    g_leaderboard.entries[idx].score = score;
    g_leaderboard.entries[idx].date = time(NULL);
    
    // 排序
    qsort(g_leaderboard.entries, g_leaderboard.count, sizeof(LeaderboardEntry), compare_entries);
    
    save_leaderboard();
}
