#include "input.h"
#include "game_logic.h"
#include "renderer.h" // for is_mouse_over

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

void handle_input(PlayerState *p1, PlayerState *p2, GameMode mode) {
    clock_t now = clock();

    if (mode == MODE_SINGLE) {
        // --- 单人模式 ---
        if (!p1->isGameOver) {
            // 旋转 (上) - 保持单次触发
            if (GetAsyncKeyState(VK_UP) & 0x8000) {
                if (!p1_rotate_held) {
                    rotate_block_action(p1);
                    p1_rotate_held = true;
                }
            } else {
                p1_rotate_held = false;
            }

            // 移动 左/右 (DAS/ARR)
            if (update_key_state(&p1->keyLeft, VK_LEFT, now, DAS_DELAY, ARR_DELAY)) {
                move_block(p1, -1, 0);
            }
            if (update_key_state(&p1->keyRight, VK_RIGHT, now, DAS_DELAY, ARR_DELAY)) {
                move_block(p1, 1, 0);
            }

            // 下落逻辑：软下落 (DAS/ARR) + 硬下落 (双击)
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
                if (!p1->keyDrop.isHeld) {
                    // 按下瞬间检测双击
                    if (now - p1_last_down_press < DOUBLE_TAP_THRESHOLD) {
                        hard_drop(p1);
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
                }
                p1->keyDrop.isHeld = true;
            } else {
                p1->keyDown.isHeld = false;
                p1->keyDrop.isHeld = false;
            }
            
            // 等级调整 (W/S) - 单次触发
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

// 设置菜单专用处理，增加滑块逻辑
// bool handle_settings_mouse(Button* buttons, int count, int* hoverIndex) { ... } 
// 已经在 main.c 中定义，此处删除或注释掉以避免重复定义
// 如果需要，可以在 input.h 中声明，并在 main.c 中实现
// 或者，将 main.c 中的实现移动到这里，并在 input.h 中声明
// 为了修复链接错误，我们暂时注释掉这里（因为 main.c 已经有了更新且包含更多逻辑的版本）

/* 
// 旧版本代码，已废弃
bool handle_settings_mouse(Button* buttons, int count, int* hoverIndex) {
    MOUSEMSG msg;
    
    while (MouseHit()) {
        msg = GetMouseMsg();
        
        // 滑块是第5个元素 (index 4)
        int sliderIdx = 4; 
        
        if (msg.uMsg == WM_MOUSEMOVE) {
            *hoverIndex = -1;
            for (int i = 0; i < count; i++) {
                if (is_mouse_over(&buttons[i], msg.x, msg.y)) {
                    *hoverIndex = i;
                    break;
                }
            }
            
            // 拖动逻辑
            if (msg.mkLButton && sliderIdx < count && is_mouse_over(&buttons[sliderIdx], msg.x, msg.y)) {
                int barX = buttons[sliderIdx].x;
                int barW = buttons[sliderIdx].w;
                // 计算相对位置比例
                int val = (msg.x - barX) * 100 / barW;
                if (val < 0) val = 0;
                if (val > 100) val = 100;
                g_settings.bgOpacity = val;
            }
        }
        else if (msg.uMsg == WM_LBUTTONDOWN) {
            // 点击直接跳转
            if (sliderIdx < count && is_mouse_over(&buttons[sliderIdx], msg.x, msg.y)) {
                int barX = buttons[sliderIdx].x;
                int barW = buttons[sliderIdx].w;
                int val = (msg.x - barX) * 100 / barW;
                if (val < 0) val = 0;
                if (val > 100) val = 100;
                g_settings.bgOpacity = val;
            }
        }
        else if (msg.uMsg == WM_LBUTTONUP) {
            for (int i = 0; i < count; i++) {
                if (is_mouse_over(&buttons[i], msg.x, msg.y)) {
                    // 如果是滑块，不返回true（不触发按钮事件）
                    if (i == sliderIdx) {
                        continue;
                    }
                    *hoverIndex = i;
                    return true;
                }
            }
        }
    }
    return false;
}
*/
