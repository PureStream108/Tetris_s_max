#include "game_logic.h"
#include "tetromino.h"

// Forward declaration
void lock_block(PlayerState *p);
void clear_lines(PlayerState *p);
void spawn_next_block(PlayerState *p);

Leaderboard g_leaderboard = {0};

// Helper to update speed based on level and difficulty
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
    
    // Set initial drop interval
    update_speed(p);
    
    p->lastDropTime = clock();
    
    // Clear board
    for (int r = 0; r < BOARD_ROWS; r++) {
        for (int c = 0; c < BOARD_COLS; c++) {
            p->board[r][c] = 0;
            p->boardColors[r][c] = BLACK;
        }
    }
    
    // Spawn initial blocks
    spawn_block(&p->currentBlock);
    spawn_block(&p->nextBlock);
}

// Check collision for a specific block state
// 碰撞检测
bool check_collision(PlayerState *p, Block *b) {
    for (int i = 0; i < 4; i++) {
        int bx = b->x + b->shape[i].x;
        int by = b->y + b->shape[i].y;
        
        // Boundaries
        if (bx < 0 || bx >= BOARD_COLS || by >= BOARD_ROWS) return true;
        
        // Occupied cells (Ignore if by < 0, meaning above board)
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
            p->board[by][bx] = 1; // Mark occupied
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
            // Shift down
            for (int nr = r; nr > 0; nr--) {
                for (int c = 0; c < BOARD_COLS; c++) {
                    p->board[nr][c] = p->board[nr - 1][c];
                    p->boardColors[nr][c] = p->boardColors[nr - 1][c];
                }
            }
            // Clear top row
            for (int c = 0; c < BOARD_COLS; c++) {
                p->board[0][c] = 0;
                p->boardColors[0][c] = BLACK;
            }
            r++; // Check this row again (since it's now the one from above)
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
        
        // Level up every 10 lines
        // Note: 如果玩家手动调整了等级，这里可能会覆盖手动调整，
        // 但通常自动升级是预期行为。如果想要完全手动控制，可以注释掉这里的升级逻辑。
        // 这里保留自动升级，但确保不超过最大等级。
        int newLevel = 1 + p->linesCleared / LINES_PER_LEVEL;
        if (newLevel > MAX_LEVEL) newLevel = MAX_LEVEL;
        
        // 只有当计算出的等级高于当前等级时才升级 (避免降级，如果用户手动调高了等级)
        if (newLevel > p->level) {
            p->level = newLevel;
            update_speed(p);
        }
    }
}

void spawn_next_block(PlayerState *p) {
    p->currentBlock = p->nextBlock;
    spawn_block(&p->nextBlock);
    
    // Check if new block collides immediately -> Game Over
    if (check_collision(p, &p->currentBlock)) {
        p->isGameOver = true;
    }
}

void update_game(PlayerState *p) {
    if (p->isGameOver || p->isPaused) {
        // Update lastDropTime so that when unpaused, it doesn't drop immediately due to large delta
        p->lastDropTime = clock();
        return;
    }
    
    clock_t now = clock();
    if (now - p->lastDropTime > p->dropInterval) {
        // Try to move down
        if (!move_block(p, 0, 1)) {
            // Landed
            lock_block(p);
            clear_lines(p);
            spawn_next_block(p);
        }
        p->lastDropTime = now;
    }
}

bool check_game_over(PlayerState *p) {
    return p->isGameOver;
}

// --- Leaderboard Implementation ---

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

// Compare for qsort (descending score)
int compare_entries(const void* a, const void* b) {
    return ((LeaderboardEntry*)b)->score - ((LeaderboardEntry*)a)->score;
}

void add_to_leaderboard(int score) {
    // If full and score is lower than lowest, ignore
    if (g_leaderboard.count == MAX_LEADERBOARD_ENTRIES && score <= g_leaderboard.entries[MAX_LEADERBOARD_ENTRIES-1].score) {
        return;
    }
    
    // Add entry
    int idx = g_leaderboard.count;
    if (idx < MAX_LEADERBOARD_ENTRIES) {
        g_leaderboard.count++;
    } else {
        idx = MAX_LEADERBOARD_ENTRIES - 1; // Overwrite last
    }
    
    // Simple default name, in real app could ask user
    strcpy(g_leaderboard.entries[idx].name, "Player");
    g_leaderboard.entries[idx].score = score;
    g_leaderboard.entries[idx].date = time(NULL);
    
    // Sort
    qsort(g_leaderboard.entries, g_leaderboard.count, sizeof(LeaderboardEntry), compare_entries);
    
    save_leaderboard();
}
