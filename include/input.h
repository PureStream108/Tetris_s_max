#ifndef INPUT_H
#define INPUT_H

#include "common.h"

// 处理双方的键盘输入
void handle_input(PlayerState *p1, PlayerState *p2, GameMode mode);

// 通用菜单鼠标处理
bool handle_mouse_menu(Button* buttons, int count, int* hoverIndex);

// 设置菜单专用鼠标处理（包含滑块逻辑）
bool handle_settings_mouse(Button* buttons, int count, int* hoverIndex);

#endif // INPUT_H
