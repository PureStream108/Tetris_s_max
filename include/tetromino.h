#ifndef TETROMINO_H
#define TETROMINO_H

#include "common.h"

// 函数原型

// 初始化一个随机类型的方块
void spawn_block(Block *block);

// 获取指定类型和旋转角度的方块相对坐标
void get_block_shape(BlockType type, int rotation, Point output[4]);

// 获取方块颜色
COLORREF get_block_color(BlockType type);

// 旋转方块（更新内部形状坐标）
void rotate_block_data(Block *block);

#endif // TETROMINO_H
