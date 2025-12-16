#include "tetromino.h"

// Definition of basic shapes (Rotation 0)
// Relative coordinates to the pivot (0,0)
// 基础形状定义（相对中心点(0,0)的坐标）
// Y is positive downwards
static const Point SHAPES[7][4] = {
    // I: Horizontal
    { {-1, 0}, {0, 0}, {1, 0}, {2, 0} },
    // O: Square
    { {0, 0}, {1, 0}, {0, 1}, {1, 1} },
    // T: T-shape pointing Up (visually) -> Y-1 is Up
    { {-1, 0}, {0, 0}, {1, 0}, {0, -1} },
    // L: L-shape
    { {-1, 0}, {0, 0}, {1, 0}, {-1, -1} }, // Top-left stick
    // J: J-shape
    { {-1, 0}, {0, 0}, {1, 0}, {1, -1} }, // Top-right stick
    // S: S-shape
    { {-1, 0}, {0, 0}, {0, -1}, {1, -1} },
    // Z: Z-shape
    { {-1, -1}, {0, -1}, {0, 0}, {1, 0} }
};

static const COLORREF COLORS[7] = {
    CYAN,       // I
    YELLOW,     // O
    MAGENTA,    // T
    RGB(255, 165, 0), // L (Orange)
    BLUE,       // J
    GREEN,      // S
    RED         // Z
};

COLORREF get_block_color(BlockType type) {
    if (type >= 0 && type < 7) return COLORS[type];
    return WHITE;
}

void get_block_shape(BlockType type, int rotation, Point output[4]) {
    // Start with base shape
    for (int i = 0; i < 4; i++) {
        output[i] = SHAPES[type][i];
    }

    // O block doesn't rotate
    if (type == BLOCK_O) return;

    // Apply rotation
    // 90 degree clockwise: x' = -y, y' = x
    for (int r = 0; r < rotation % 4; r++) {
        for (int i = 0; i < 4; i++) {
            int tmpX = output[i].x;
            output[i].x = -output[i].y;
            output[i].y = tmpX;
        }
    }
}

void spawn_block(Block *block) {
    block->type = (BlockType)(rand() % 7);
    block->rotation = 0; // rand() % 4; // Usually start flat
    block->x = BOARD_COLS / 2 - 1; // Center roughly
    block->y = 0; // Top
    block->color = get_block_color(block->type);
    
    get_block_shape(block->type, block->rotation, block->shape);
}

void rotate_block_data(Block *block) {
    block->rotation = (block->rotation + 1) % 4;
    get_block_shape(block->type, block->rotation, block->shape);
}
