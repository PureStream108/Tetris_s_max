#include "include/input.h"
#include "include/game_logic.h"
#include "include/renderer.h" // for is_mouse_over
#include "include/levels.h" // 引入 LevelID 和 LevelData 定义
#include "include/tetromino.h" // 引入 spawn_block 声明

// 按键状态跟踪，用于“按一次触发一次”的动作（如旋转）
static bool p1_rotate_held = false;
static bool p2_rotate_held = false;
// 等级调整按键跟踪
static bool p1_level_up_held = false;
static bool p1_level_down_held = false;

// DAS/ARR 配置
#define DAS_DELAY 200 // 延迟自动移动 (Delayed Auto Shift) - ms
#define ARR_DELAY 50  // 自动重复速率 (Auto Repeat Rate) - ms
#define SOFT_DROP_ARR 30 // 软下落重复速率 - ms

// 双击下落检测
static clock_t p1_last_down_press = 0;
static clock_t p2_last_down_press = 0;
#define DOUBLE_TAP_THRESHOLD 250 // ms

// 辅助函数：更新按键状态并返回是否触发动作
bool update_key_state(KeyState* state, int vKey, clock_t now, int das, int arr) {
    if (GetAsyncKeyState(vKey) & 0x8000) {
        if (!state->isHeld) {
            // 第一次按下
            state->isHeld = true;
            state->firstPressTime = now;
            state->lastRepeatTime = now;
            return true; // 立即触发一次
        } else {
            // 按住中
            if (now - state->firstPressTime > das) {
                // 已过 DAS 延迟，开始连发
                if (now - state->lastRepeatTime > arr) {
                    state->lastRepeatTime = now;
                    return true; // 触发连发
                }
            }
        }
    } else {
        // 松开
        state->isHeld = false;
    }
    return false;
}

// R键重开检测
static bool r_key_held = false;

extern void init_player(PlayerState *p, int id); // 重新初始化玩家声明
extern GameState g_gameState; // 全局状态
extern LevelData g_levels[]; // 关卡数据

void handle_input(PlayerState *p1, PlayerState *p2, GameMode mode) {
    clock_t now = clock();

    if (mode == MODE_SINGLE || mode == MODE_LEVEL) {
        // --- 单人模式 或 关卡模式 ---
        
        // bool isMirrorMode = (mode == MODE_LEVEL && p2 != NULL); 
        bool isMirrorMode = false;
        
        // R键重开逻辑
        if (GetAsyncKeyState('R') & 0x8000) {
            if (!r_key_held && !p1->isGameOver && !p1->isWinner && !p1->isPaused) {
                // 执行重开
                if (mode == MODE_LEVEL) {
                    // 关卡模式：使用 start_level 统一初始化 (包含 restrictPieces 处理)
                    start_level(p1, 1, p1->currentLevelIndex);
                } else {
                    // 普通模式
                    init_player(p1, 1);
                }
                
                r_key_held = true;
            }
        } else {
            r_key_held = false;
        }

        if (!p1->isGameOver && !p1->isWinner) {
            // 旋转 (上) - 保持单次触发
            if (GetAsyncKeyState(VK_UP) & 0x8000) {
                if (!p1_rotate_held) {
                    rotate_block_action(p1);
                    if (isMirrorMode && !p2->isGameOver) rotate_block_action(p2);
                    p1_rotate_held = true;
                }
            } else {
                p1_rotate_held = false;
            }

            // 移动 左/右 (DAS/ARR)
            if (update_key_state(&p1->keyLeft, VK_LEFT, now, DAS_DELAY, ARR_DELAY)) {
                move_block(p1, -1, 0);
                if (isMirrorMode && !p2->isGameOver) move_block(p2, -1, 0);
            }
            if (update_key_state(&p1->keyRight, VK_RIGHT, now, DAS_DELAY, ARR_DELAY)) {
                move_block(p1, 1, 0);
                if (isMirrorMode && !p2->isGameOver) move_block(p2, 1, 0);
            }

            // 下落逻辑：软下落 (DAS/ARR) + 硬下落 (双击)
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
                if (!p1->keyDrop.isHeld) {
                    // 按下瞬间检测双击
                    if (now - p1_last_down_press < DOUBLE_TAP_THRESHOLD) {
                        hard_drop(p1);
                        if (isMirrorMode && !p2->isGameOver) hard_drop(p2);
                        p1_last_down_press = 0; // 重置
                        // 硬下落后通常不应立即触发软下落，这里简单处理即可
                    } else {
                        // 记录本次按下时间
                        p1_last_down_press = now;
                    }
                }
                // 使用 update_key_state 处理软下落连发
                // 注意：软下落通常 DAS 较短或没有，这里沿用标准 DAS
                if (update_key_state(&p1->keyDown, VK_DOWN, now, DAS_DELAY, SOFT_DROP_ARR)) {
                    move_block(p1, 0, 1);
                    if (isMirrorMode && !p2->isGameOver) move_block(p2, 0, 1);
                }
                p1->keyDrop.isHeld = true;
            } else {
                p1->keyDown.isHeld = false;
                p1->keyDrop.isHeld = false;
            }
            
            // 等级调整 (W/S) - 仅在普通单人模式下启用，关卡模式禁用
            if (mode == MODE_SINGLE) {
                // W: 增加等级
                if (GetAsyncKeyState('W') & 0x8000) {
                    if (!p1_level_up_held) {
                        if (p1->level < MAX_LEVEL) {
                            p1->level++;
                            update_speed(p1);
                        }
                        p1_level_up_held = true;
                    }
                } else {
                    p1_level_up_held = false;
                }
                
                // S: 减少等级
                if (GetAsyncKeyState('S') & 0x8000) {
                    if (!p1_level_down_held) {
                        if (p1->level > 1) {
                            p1->level--;
                            update_speed(p1);
                        }
                        p1_level_down_held = true;
                    }
                } else {
                    p1_level_down_held = false;
                }
            }
        }
    } else {
        // --- 双人模式 ---
        
        // 玩家 1 (左) - WASD
        if (!p1->isGameOver) {
            // 旋转 (W)
            if (GetAsyncKeyState('W') & 0x8000) {
                if (!p1_rotate_held) {
                    rotate_block_action(p1);
                    p1_rotate_held = true;
                }
            } else {
                p1_rotate_held = false;
            }

            // 移动 A/D (DAS/ARR)
            if (update_key_state(&p1->keyLeft, 'A', now, DAS_DELAY, ARR_DELAY)) {
                move_block(p1, -1, 0);
            }
            if (update_key_state(&p1->keyRight, 'D', now, DAS_DELAY, ARR_DELAY)) {
                move_block(p1, 1, 0);
            }

            // 下落 (S)：双击 + 软下落
            if (GetAsyncKeyState('S') & 0x8000) {
                if (!p1->keyDrop.isHeld) {
                    if (now - p1_last_down_press < DOUBLE_TAP_THRESHOLD) {
                        hard_drop(p1);
                        p1_last_down_press = 0;
                    } else {
                        p1_last_down_press = now;
                    }
                }
                if (update_key_state(&p1->keyDown, 'S', now, DAS_DELAY, SOFT_DROP_ARR)) {
                    move_block(p1, 0, 1);
                }
                p1->keyDrop.isHeld = true;
            } else {
                p1->keyDown.isHeld = false;
                p1->keyDrop.isHeld = false;
            }
        }

        // 玩家 2 (右) - 方向键
        if (p2 && !p2->isGameOver) {
            // 旋转 (上)
            if (GetAsyncKeyState(VK_UP) & 0x8000) {
                if (!p2_rotate_held) {
                    rotate_block_action(p2);
                    p2_rotate_held = true;
                }
            } else {
                p2_rotate_held = false;
            }

            // 移动 左/右 (DAS/ARR)
            if (update_key_state(&p2->keyLeft, VK_LEFT, now, DAS_DELAY, ARR_DELAY)) {
                move_block(p2, -1, 0);
            }
            if (update_key_state(&p2->keyRight, VK_RIGHT, now, DAS_DELAY, ARR_DELAY)) {
                move_block(p2, 1, 0);
            }

            // 下落 (下)：双击 + 软下落
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
                if (!p2->keyDrop.isHeld) {
                    if (now - p2_last_down_press < DOUBLE_TAP_THRESHOLD) {
                        hard_drop(p2);
                        p2_last_down_press = 0;
                    } else {
                        p2_last_down_press = now;
                    }
                }
                if (update_key_state(&p2->keyDown, VK_DOWN, now, DAS_DELAY, SOFT_DROP_ARR)) {
                    move_block(p2, 0, 1);
                }
                p2->keyDrop.isHeld = true;
            } else {
                p2->keyDown.isHeld = false;
                p2->keyDrop.isHeld = false;
            }
        }
    }
}

bool handle_mouse_menu(Button* buttons, int count, int* hoverIndex) {
    MOUSEMSG msg;
    
    // 检查是否有鼠标消息
    while (MouseHit()) {
        msg = GetMouseMsg();
        
        if (msg.uMsg == WM_MOUSEMOVE) {
            *hoverIndex = -1;
            for (int i = 0; i < count; i++) {
                if (is_mouse_over(&buttons[i], msg.x, msg.y)) {
                    *hoverIndex = i;
                    break;
                }
            }
        } else if (msg.uMsg == WM_LBUTTONUP) {
            for (int i = 0; i < count; i++) {
                if (is_mouse_over(&buttons[i], msg.x, msg.y)) {
                    *hoverIndex = i; // 确保选中
                    return true;
                }
            }
        }
    }
    return false;
}
