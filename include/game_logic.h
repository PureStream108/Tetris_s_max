#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "common.h"

// 初始化玩家状态
void init_player(PlayerState *p, int id);

// 游戏逻辑更新（重力下落等）
void update_game(PlayerState *p);

// 输入触发的动作
bool move_block(PlayerState *p, int dx, int dy); // Returns true if moved
void rotate_block_action(PlayerState *p);

// 检查游戏结束
bool check_game_over(PlayerState *p);

// 根据等级更新下落速度
void update_speed(PlayerState *p);

void load_leaderboard();
void save_leaderboard();
void add_to_leaderboard(int score);

#endif // GAME_LOGIC_H
