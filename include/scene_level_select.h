#ifndef SCENE_LEVEL_SELECT_H
#define SCENE_LEVEL_SELECT_H

#include "common.h"
#include "levels.h"

// 初始化选关场景
void init_level_select();

// 更新选关逻辑 (拖拽等)
void update_level_select();

// 绘制选关场景
void draw_level_select();

// 处理输入
void handle_level_select_input(ExMessage *msg);

#endif // SCENE_LEVEL_SELECT_H
