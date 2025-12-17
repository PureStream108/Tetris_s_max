#include "include/tetromino.h"
#include <stdlib.h> // for rand

// 基础形状定义 (旋转 0)
// 相对中心点 (0,0) 的坐标
// Y 轴向下为正
static const Point SHAPES[7][4] = {
    // I: 水平
    { {-1, 0}, {0, 0}, {1, 0}, {2, 0} },
    // O: 正方形
    { {0, 0}, {1, 0}, {0, 1}, {1, 1} },
    // T: T型 朝上 (视觉上) -> Y-1 是上
    { {-1, 0}, {0, 0}, {1, 0}, {0, -1} },
    // L: L型
    { {-1, 0}, {0, 0}, {1, 0}, {-1, -1} }, // 左上角
    // J: J型
    { {-1, 0}, {0, 0}, {1, 0}, {1, -1} }, // 右上角
    // S: S型
    { {-1, 0}, {0, 0}, {0, -1}, {1, -1} },
    // Z: Z型
    { {-1, -1}, {0, -1}, {0, 0}, {1, 0} }
};

static const COLORREF COLORS[7] = {
    CYAN,       // I
    YELLOW,     // O
    MAGENTA,    // T
    RGB(255, 165, 0), // L (橙色)
    BLUE,       // J
    GREEN,      // S
    RED         // Z
};

COLORREF get_block_color(BlockType type) {
    if (type >= 0 && type < 7) return COLORS[type];
    return WHITE;
}

void get_block_shape(BlockType type, int rotation, Point output[4]) {
    // 从基础形状开始
    for (int i = 0; i < 4; i++) {
        output[i] = SHAPES[type][i];
    }

    // O 方块不旋转
    if (type == BLOCK_O) return;

    // 应用旋转
    // 顺时针 90 度: x' = -y, y' = x
    for (int r = 0; r < rotation % 4; r++) {
        for (int i = 0; i < 4; i++) {
            int tmpX = output[i].x;
            output[i].x = -output[i].y;
            output[i].y = tmpX;
        }
    }
}

void spawn_block(Block *block, bool restrictPieces) {
    if (restrictPieces) {
        // 只生成 S (5) 和 Z (6)
        block->type = (rand() % 2 == 0) ? BLOCK_S : BLOCK_Z;
    } else {
        block->type = (BlockType)(rand() % 7);
    }
    
    block->rotation = 0; // 通常平放开始
    block->x = BOARD_COLS / 2 - 1; // 大致居中
    block->y = 0; // 顶部
    block->color = get_block_color(block->type);
    
    get_block_shape(block->type, block->rotation, block->shape);
}

void rotate_block_data(Block *block) {
    block->rotation = (block->rotation + 1) % 4;
    get_block_shape(block->type, block->rotation, block->shape);
}
