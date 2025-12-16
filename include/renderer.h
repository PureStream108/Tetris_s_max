#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"

// 初始化图形窗口
void init_game_window();

// 关闭图形窗口
void close_game_window();

void update_window_size();

void reload_background();

//  绘制图选
void draw_intro();

void render_game(PlayerState *p1, PlayerState *p2, GameMode mode, int timeLeft, int gameTime, Button* pauseButtons, int pauseBtnCount, int pauseHoverIdx);

// 绘制开始菜单
void draw_menu(Button* buttons, int buttonCount, int hoverIndex);

// 绘制菜单
void draw_settings(Button* buttons, int buttonCount, int hoverIndex);

// 绘制按钮 (供内部和外部使用)
void draw_button(const Button* btn, bool isHover);

// 绘制排行榜
void draw_leaderboard(Button* buttons, int buttonCount, int hoverIndex);

// 绘制结束画面
void draw_game_over(PlayerState *p1, PlayerState *p2, GameMode mode);

bool is_mouse_over(const Button* btn, int mx, int my);

// 绘制音乐滚动条
void draw_music_ticker();

#endif // RENDERER_H
