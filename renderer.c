#include "include/renderer.h"
#include "include/tetromino.h"
#include "include/levels.h"

// 链接 Msimg32.lib 以使用 AlphaBlend 函数
#pragma comment(lib, "Msimg32.lib")

// 全局缩放变量
int g_blockSize = 30;
int g_windowWidth = BASE_WINDOW_WIDTH;
int g_windowHeight = BASE_WINDOW_HEIGHT;
TCHAR g_currentMusicTitle[256] = _T(""); // 初始化为空

static IMAGE* g_bgImage = NULL;

// 缓存的黑色遮罩 GDI 资源，防止每帧创建销毁导致闪烁
static HDC g_overlayMemDC = NULL;
static HBITMAP g_overlayBitmap = NULL;
static HBITMAP g_overlayOldBitmap = NULL;

// 辅助函数：应用可调整大小的窗口样式 (修正：调整窗口大小以保持客户区)
static void apply_resize_style() {
    HWND hwnd = GetHWnd();
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    
    // 添加 WS_THICKFRAME (可调整边框) 和 WS_MAXIMIZEBOX (最大化按钮)
    style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    SetWindowLong(hwnd, GWL_STYLE, style);
    
    // 计算包含新样式（边框等）所需的窗口总尺寸
    // 目标客户区大小为 g_windowWidth x g_windowHeight
    RECT rc = {0, 0, g_windowWidth, g_windowHeight};
    // AdjustWindowRect 自动计算加上边框、标题栏后的尺寸
    // FALSE 表示没有菜单栏
    AdjustWindowRect(&rc, style, FALSE);
    
    int totalWidth = rc.right - rc.left;
    int totalHeight = rc.bottom - rc.top;
    
    // 强制刷新窗口框架并设置正确的外部尺寸，避免客户区被挤压变小
    // 注意：这里去掉了 SWP_NOSIZE，因为我们需要调整外部尺寸
    SetWindowPos(hwnd, NULL, 0, 0, totalWidth, totalHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

// 释放遮罩资源
void cleanup_overlay_resources() {
    if (g_overlayMemDC) {
        SelectObject(g_overlayMemDC, g_overlayOldBitmap);
        DeleteDC(g_overlayMemDC);
        g_overlayMemDC = NULL;
    }
    if (g_overlayBitmap) {
        DeleteObject(g_overlayBitmap);
        g_overlayBitmap = NULL;
    }
}

// 初始化遮罩资源 (1x1 黑色像素)
void init_overlay_resources() {
    cleanup_overlay_resources(); // 先清理旧的
    
    HDC hdc = GetImageHDC(NULL);
    g_overlayMemDC = CreateCompatibleDC(hdc);
    g_overlayBitmap = CreateCompatibleBitmap(hdc, 1, 1);
    g_overlayOldBitmap = (HBITMAP)SelectObject(g_overlayMemDC, g_overlayBitmap);
    SetPixel(g_overlayMemDC, 0, 0, RGB(0,0,0));
}

// 绘制半透明黑色遮罩的辅助函数 (优化：使用缓存资源)
// alpha: 0 (完全透明) 到 255 (完全不透明/纯黑)
void draw_dim_overlay(int alpha) {
    if (alpha <= 0) return;
    if (alpha > 255) alpha = 255;
    
    // 确保资源已初始化
    if (!g_overlayMemDC) {
        init_overlay_resources();
    }
    
    HDC hdc = GetImageHDC(NULL);
    
    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = alpha;
    bf.AlphaFormat = 0;
    
    // 将黑色像素拉伸覆盖整个窗口，应用 Alpha 混合
    AlphaBlend(hdc, 0, 0, g_windowWidth, g_windowHeight, g_overlayMemDC, 0, 0, 1, 1, bf);
}

void init_game_window() {
    initgraph(g_windowWidth, g_windowHeight);
    setbkmode(TRANSPARENT); 
    settextstyle(20, 0, _T("SimHei")); // 使用黑体
    
    // 初始化遮罩资源
    init_overlay_resources();
    
    // 确保窗口样式允许调整大小
    apply_resize_style();
    
    reload_background();
}

void reload_background() {
    if (g_bgImage) {
        delete g_bgImage;
        g_bgImage = NULL;
    }
    
    if (wcslen(g_settings.bgImagePath) > 0) {
        g_bgImage = new IMAGE();
        // 加载图片并自动缩放到当前窗口大小
        loadimage(g_bgImage, g_settings.bgImagePath, g_windowWidth, g_windowHeight, true);
    }
}

void update_window_size() {
    // 关闭窗口前清理资源
    cleanup_overlay_resources();
    
    closegraph();
    initgraph(g_windowWidth, g_windowHeight);
    setbkmode(TRANSPARENT);
    
    // 重新初始化遮罩资源
    init_overlay_resources();
    
    // 重新创建窗口后，必须再次应用样式
    apply_resize_style();
    
    reload_background(); // 重新加载以适应新尺寸
}

void close_game_window() {
    cleanup_overlay_resources();
    if (g_bgImage) delete g_bgImage;
    closegraph();
}

void draw_background() {
    if (g_bgImage) {
        putimage(0, 0, g_bgImage);
        
        // 根据设置的不透明度绘制遮罩
        // bgOpacity 100: 原图 (alpha=0)
        // bgOpacity 0: 纯黑 (alpha=255)
        int alpha = 255 * (100 - g_settings.bgOpacity) / 100;
        draw_dim_overlay(alpha);
    } else {
        cleardevice();
    }
}

void draw_intro() {
    settextstyle(50, 0, _T("SimHei"));
    const TCHAR* title = _T("Tetris Su7 Max by mimimiHoYo");
    int tw = textwidth(title);
    int th = textheight(title);
    int x = (g_windowWidth - tw) / 2;
    int y = (g_windowHeight - th) / 2;

    BeginBatchDraw();
    for (int i = 0; i <= 255; i += 5) {
        draw_background(); // 绘制背景
        settextcolor(RGB(i, i, i));
        outtextxy(x, y, title);
        FlushBatchDraw();
        Sleep(20);
    }
    Sleep(1000);
    for (int i = 255; i >= 0; i -= 5) {
        draw_background();
        settextcolor(RGB(i, i, i));
        outtextxy(x, y, title);
        FlushBatchDraw();
        Sleep(20);
    }
    EndBatchDraw();
}

void draw_cell(int x, int y, COLORREF color) {
    setfillcolor(color);
    setlinecolor(COLOR_BORDER);
    fillrectangle(x, y, x + g_blockSize, y + g_blockSize);
}

void draw_block(Block *b, int startX, int startY, bool isPreview) {
    for (int i = 0; i < 4; i++) {
        int bx, by;
        if (isPreview) {
            bx = b->shape[i].x + 1; 
            by = b->shape[i].y + 1;
        } else {
            bx = b->x + b->shape[i].x;
            by = b->y + b->shape[i].y;
        }
        
        if (!isPreview && by < 0) continue;
        
        int screenX = startX + bx * g_blockSize;
        int screenY = startY + by * g_blockSize;
        
        draw_cell(screenX, screenY, b->color);
    }
}

void draw_board(PlayerState *p, int startX, int startY) {
    setlinecolor(WHITE);
    // 绘制棋盘背景（黑色），确保方块可见
    // 仅绘制边框，内部透明显示背景
    rectangle(startX - 1, startY - 1, startX + BOARD_COLS * g_blockSize + 1, startY + BOARD_ROWS * g_blockSize + 1);
    
    // 如果想要棋盘内背景半透明黑，可以在这里再画一个半透明遮罩
    // 但目前保持透明
    
    for (int r = 0; r < BOARD_ROWS; r++) {
        for (int c = 0; c < BOARD_COLS; c++) {
            if (p->board[r][c] != 0) {
                int screenX = startX + c * g_blockSize;
                int screenY = startY + r * g_blockSize;
                draw_cell(screenX, screenY, p->boardColors[r][c]);
            }
        }
    }
    draw_block(&p->currentBlock, startX, startY, false);
}

void draw_info(PlayerState *p, int startX, int startY) {
    TCHAR buf[64];
    settextcolor(COLOR_TEXT);
    settextstyle(20, 0, _T("SimHei"));
    setbkmode(TRANSPARENT);
    
    _stprintf_s(buf, 64, _T("Score: %d"), p->score);
    outtextxy(startX, startY, buf);
    
    _stprintf_s(buf, 64, _T("Level: %d"), p->level);
    outtextxy(startX, startY + 30, buf);
    
    // 关卡模式显示目标剩余行数
    extern GameState g_gameState;
    if (g_gameState == STATE_GAME_LEVEL) {
        int left = p->levelTargetLines - p->linesCleared;
        if (left < 0) left = 0;
        _stprintf_s(buf, 64, _T("Left: %d"), left);
        outtextxy(startX, startY + 60, buf);
    } else {
        _stprintf_s(buf, 64, _T("Lines: %d"), p->linesCleared);
        outtextxy(startX, startY + 60, buf);
    }
    
    outtextxy(startX, startY + 100, _T("Next:"));
    draw_block(&p->nextBlock, startX, startY + 130, true);
}

// 绘制按键提示 (单人模式左侧)
void draw_key_guide(int startX, int startY) {
    settextcolor(WHITE);
    settextstyle(18, 0, _T("SimHei"));
    setbkmode(TRANSPARENT);
    
    int lineHeight = 25;
    outtextxy(startX, startY, _T("--- 操作指南 ---"));
    outtextxy(startX, startY + lineHeight * 1, _T("↑: 旋转 (Rotate)"));
    outtextxy(startX, startY + lineHeight * 2, _T("←/→: 移动 (Move)"));
    outtextxy(startX, startY + lineHeight * 3, _T("↓: 加速 (Drop)"));
    outtextxy(startX, startY + lineHeight * 4, _T("↓↓(短按两次): 快速落地 (Drop)"));
    outtextxy(startX, startY + lineHeight * 5, _T("W/S: 调整等级 (Level)"));
    outtextxy(startX, startY + lineHeight * 6, _T("Space: 暂停 (Pause)"));
    outtextxy(startX, startY + lineHeight * 7, _T("ESC: 返回 (Back)"));
    outtextxy(startX, startY + lineHeight * 8, _T("R: 重新开始 (Restart)"));
}

// 绘制暂停界面遮罩
void draw_pause_overlay(Button* buttons, int count, int hoverIndex) {
    // 全屏半透明黑色遮罩
    draw_dim_overlay(150);
    
    settextcolor(YELLOW);
    settextstyle(40, 0, _T("SimHei"));
    setbkmode(TRANSPARENT);
    const TCHAR* msg = _T("PAUSED");
    int tw = textwidth(msg);
    int th = textheight(msg);
    
    int cx = g_windowWidth / 2;
    int cy = g_windowHeight / 2;
    
    // 标题
    outtextxy(cx - tw/2, cy - 100, msg);
    
    // 绘制暂停菜单按钮
    for (int i = 0; i < count; i++) {
        draw_button(&buttons[i], i == hoverIndex);
    }
}

// 绘制音乐滚动条
void draw_music_ticker() {
    if (_tcslen(g_currentMusicTitle) == 0 || !g_settings.musicEnabled) return;
    
    static clock_t lastTick = 0;
    static int offsetX = 0;
    clock_t now = clock();
    
    // 每 20ms 移动一次像素
    if (now - lastTick > 20) {
        offsetX += 1;
        lastTick = now;
    }
    
    settextstyle(16, 0, _T("SimHei"));
    TCHAR displayStr[300];
    _stprintf_s(displayStr, 300, _T("正在播放 (Now Playing): %s   "), g_currentMusicTitle);
    
    int textW = textwidth(displayStr);
    int barH = 24;
    
    // 重置偏移量
    if (offsetX > textW + g_windowWidth) {
        offsetX = -g_windowWidth;
    }
    
    // 绘制顶部半透明黑条
    setfillcolor(RGB(30, 30, 30));
    setlinecolor(RGB(30, 30, 30));
    fillrectangle(0, 0, g_windowWidth, barH);
    
    settextcolor(RGB(100, 255, 100)); // 亮绿色文字
    setbkmode(TRANSPARENT);
    
    // 从右往左滚动
    int drawX = g_windowWidth - offsetX;
    
    outtextxy(drawX, 4, displayStr);
}

void render_game(PlayerState *p1, PlayerState *p2, GameMode mode, int timeLeft, int gameTime, Button* pauseButtons, int pauseBtnCount, int pauseHoverIdx) {
    BeginBatchDraw();
    draw_background();
    
    int sp_x = g_windowWidth / 2 - (BOARD_COLS * g_blockSize) / 2;
    int sp_y = 50;
    
    int p1_x = g_windowWidth / 4 - (BOARD_COLS * g_blockSize) / 2;
    int p2_x = (g_windowWidth * 3) / 4 - (BOARD_COLS * g_blockSize) / 2;
    
    bool isPaused = false;
    
    // 检查是否为 OS-3 (双人控制) - 已删除
    /*
    bool isDualControl = false;
    if (mode == MODE_LEVEL) {
        LevelData* level = get_level_data((LevelID)p1->currentLevelIndex);
        if (level && level->dualControl) {
            isDualControl = true;
        }
    }
    */
    bool isDualControl = false; // 强制为 false
    
    if ((mode == MODE_SINGLE || mode == MODE_LEVEL) && !isDualControl) { // 单人或关卡模式
        draw_board(p1, sp_x, sp_y);
        draw_info(p1, sp_x + BOARD_COLS * g_blockSize + 20, sp_y);
        
        // 绘制左侧按键指南
        draw_key_guide(50, 100);
        
        if (p1->isPaused) isPaused = true;

        // 关卡模式浮动标题动画
        if (mode == MODE_LEVEL) {
            clock_t now = clock();
            double elapsed = (double)(now - p1->levelStartTime) / CLOCKS_PER_SEC;
            
            // 动画时长：前0.5s淡入，停留2s，后0.5s淡出，总共3s
            if (elapsed < 3.0) {
                int alpha = 0;
                if (elapsed < 0.5) {
                    alpha = (int)(elapsed / 0.5 * 255);
                } else if (elapsed < 2.5) {
                    alpha = 255;
                } else {
                    alpha = (int)((3.0 - elapsed) / 0.5 * 255);
                }
                
                // 绘制标题
                LevelData* level = get_level_data((LevelID)p1->currentLevelIndex);
                if (level) {
                    settextstyle(60, 0, _T("SimHei"));
                    
                    // 手动模拟 alpha 颜色 (白字)
                    // 背景是动态的，所以最好不要用 settextcolor(RGB(alpha,alpha,alpha))，这只是变灰
                    // 真正的 alpha 混合需要 buffer，这里简单地用灰色模拟淡入淡出
                    settextcolor(RGB(alpha, alpha, alpha)); 
                    
                    TCHAR wTitle[64];
                    #ifdef UNICODE
                        _tcscpy(wTitle, level->title);
                    #else
                        strcpy(wTitle, level->title);
                    #endif
                    
                    int tx = (g_windowWidth - textwidth(wTitle)) / 2;
                    int ty = g_windowHeight / 2 - 100;
                    
                    setbkmode(TRANSPARENT);
                    outtextxy(tx, ty, wTitle);
                }
            }
        }

    } else {
        // 双人模式布局调整 (或 OS-3)
        // 完全基于窗口宽度的百分比，适应任何分辨率
        int boardWidth = BOARD_COLS * g_blockSize;
        
        // P1: 居中于左侧 25% 区域
        int p1_final_x = (g_windowWidth * 25 / 100) - (boardWidth / 2);
        
        // P2: 居中于右侧 75% 区域
        int p2_final_x = (g_windowWidth * 75 / 100) - (boardWidth / 2);
        
        // 信息栏位置：相对棋盘偏移
        // P1 信息栏在棋盘左侧
        draw_board(p1, p1_final_x, sp_y);
        draw_info(p1, p1_final_x - 140, sp_y); 
        
        // P2 信息栏在棋盘右侧
        if (p2) {
             draw_board(p2, p2_final_x, sp_y);
             draw_info(p2, p2_final_x + boardWidth + 20, sp_y);
        }
        
        // 显示时间 (仅在非 OS-3 模式下显示对战时间，OS-3 显示其他信息或不显示时间)
        if (!isDualControl) {
            TCHAR timeBuf[32];
            if (timeLeft == -1) {
                _stprintf_s(timeBuf, 32, _T("Time: ∞"));
            } else {
                _stprintf_s(timeBuf, 32, _T("Time: %ds"), timeLeft);
            }
            
            settextstyle(30, 0, _T("SimHei"));
            settextcolor(LIGHTRED);
            outtextxy(g_windowWidth / 2 - 60, 20, timeBuf);
        } else {
            // OS-3 标题 (已删除)
            /*
            settextstyle(30, 0, _T("SimHei"));
            settextcolor(YELLOW);
            const TCHAR* title = _T("PM-OS-3 同调");
            outtextxy((g_windowWidth - textwidth(title)) / 2, 20, title);
            */
        }
        
        if (p1->isPaused || (p2 && p2->isPaused)) isPaused = true;
    }
    
    // 绘制音乐滚动条
    draw_music_ticker();
    
    if (isPaused) {
        draw_pause_overlay(pauseButtons, pauseBtnCount, pauseHoverIdx);
    }

    // 绘制关卡胜利/失败弹窗 (如果处于该状态)
    extern GameState g_gameState;
    if (g_gameState == STATE_GAME_LEVEL_WIN) {
        draw_dim_overlay(150);
        
        const TCHAR* msg;
        // PM-10 胜利显示特殊文案
        if (p1->currentLevelIndex == LEVEL_10) {
             // 字体稍微调小一点防止溢出
            settextstyle(40, 0, _T("SimHei")); 
            settextcolor(GREEN);
            msg = _T("0xGame{C@ll_PureSt34m_QQ_t0_Get_Vme50}");
        } else {
            settextstyle(60, 0, _T("SimHei"));
            settextcolor(GREEN);
            msg = _T("Congratulations!");
        }

        outtextxy((g_windowWidth - textwidth(msg))/2, g_windowHeight/2 - 50, msg);
        
        settextstyle(24, 0, _T("SimHei"));
        settextcolor(WHITE);
        const TCHAR* sub = _T("按 ENTER 继续 / ESC 返回");
        outtextxy((g_windowWidth - textwidth(sub))/2, g_windowHeight/2 + 50, sub);
    }
    
    EndBatchDraw();
}

void draw_button(const Button* btn, bool isHover) {
    // 设置半透明深色底
    COLORREF fillColor = isHover ? RGB(80, 80, 80) : RGB(30, 30, 30);
    COLORREF borderColor = isHover ? RGB(255, 255, 0) : RGB(200, 200, 200); // Hover 时边框变黄
    
    setfillcolor(fillColor);
    setlinecolor(borderColor);
    
    // 使用圆角矩形
    int r = 10; // 圆角半径
    fillroundrect(btn->x, btn->y, btn->x + btn->w, btn->y + btn->h, r, r);
    
    // 如果是 Hover，画一个稍微粗一点的框
    if (isHover) {
        setlinestyle(PS_SOLID, 2);
        roundrect(btn->x, btn->y, btn->x + btn->w, btn->y + btn->h, r, r);
        setlinestyle(PS_SOLID, 1); // 还原
    } else {
        roundrect(btn->x, btn->y, btn->x + btn->w, btn->y + btn->h, r, r);
    }
    
    settextcolor(COLOR_BUTTON_TEXT);
    settextstyle(24, 0, _T("SimHei"));
    
    // 文字居中
    int tw = textwidth(btn->text);
    int th = textheight(btn->text);
    int tx = btn->x + (btn->w - tw) / 2;
    int ty = btn->y + (btn->h - th) / 2;
    
    setbkmode(TRANSPARENT);
    outtextxy(tx, ty, btn->text);
}

// 绘制设置菜单中的滑块
void draw_opacity_slider(const Button* btn, int opacity) {
    // 标题
    TCHAR buf[32];
    _stprintf_s(buf, 32, _T("背景不透明度: %d%%"), opacity);
    settextcolor(WHITE);
    settextstyle(24, 0, _T("SimHei"));
    outtextxy(btn->x, btn->y, buf);
    
    // 滑动条区域
    int barX = btn->x;
    int barY = btn->y + 35;
    int barW = btn->w;
    int barH = 10;
    
    // 画槽
    setfillcolor(RGB(100, 100, 100));
    fillroundrect(barX, barY, barX + barW, barY + barH, 5, 5);
    
    // 画滑块
    int handleW = 20;
    int handleH = 20;
    int handleX = barX + (barW * opacity / 100) - (handleW / 2);
    int handleY = barY + (barH - handleH) / 2;
    
    // 限制滑块在槽内
    if (handleX < barX) handleX = barX;
    if (handleX > barX + barW - handleW) handleX = barX + barW - handleW;
    
    setfillcolor(WHITE);
    fillroundrect(handleX, handleY, handleX + handleW, handleY + handleH, 5, 5);
}

void draw_menu(Button* buttons, int buttonCount, int hoverIndex) {
    BeginBatchDraw();
    draw_background();
    
    // 增加全局半透明遮罩，提高文字可读性
    draw_dim_overlay(100); 
    
    settextstyle(70, 0, _T("SimHei")); // 字体加大
    settextcolor(YELLOW);
    const TCHAR* title = _T("Tetris Su7 Max");
    // 调整标题位置，略微上移
    outtextxy((g_windowWidth - textwidth(title))/2, 80, title);
    
    for (int i = 0; i < buttonCount; i++) {
        draw_button(&buttons[i], i == hoverIndex);
    }
    
    // 菜单也显示音乐条
    draw_music_ticker();
    
    EndBatchDraw();
}

// 绘制时间选择菜单
void draw_time_select_menu(Button* buttons, int buttonCount, int hoverIndex) {
    BeginBatchDraw();
    draw_background();
    draw_dim_overlay(100);
    
    settextstyle(50, 0, _T("SimHei"));
    settextcolor(WHITE);
    const TCHAR* title = _T("选择对战时间");
    outtextxy((g_windowWidth - textwidth(title))/2, 100, title);
    
    for (int i = 0; i < buttonCount; i++) {
        draw_button(&buttons[i], i == hoverIndex);
    }
    
    draw_music_ticker();
    
    EndBatchDraw();
}

// 绘制音量滑块
void draw_volume_slider(const Button* btn, int volume) {
    // 标题
    TCHAR buf[32];
    _stprintf_s(buf, 32, _T("音乐音量: %d%%"), volume / 10); // 0-1000 -> 0-100
    settextcolor(WHITE);
    settextstyle(24, 0, _T("SimHei"));
    outtextxy(btn->x, btn->y, buf);
    
    // 滑动条区域
    int barX = btn->x;
    int barY = btn->y + 35;
    int barW = btn->w;
    int barH = 10;
    
    // 画槽
    setfillcolor(RGB(100, 100, 100));
    fillroundrect(barX, barY, barX + barW, barY + barH, 5, 5);
    
    // 画滑块
    int handleW = 20;
    int handleH = 20;
    // volume 是 0-1000，映射到 0-1
    int handleX = barX + (int)((double)barW * volume / 1000.0) - (handleW / 2);
    int handleY = barY + (barH - handleH) / 2;
    
    // 限制滑块在槽内
    if (handleX < barX) handleX = barX;
    if (handleX > barX + barW - handleW) handleX = barX + barW - handleW;
    
    setfillcolor(WHITE);
    fillroundrect(handleX, handleY, handleX + handleW, handleY + handleH, 5, 5);
}

void draw_settings(Button* buttons, int buttonCount, int hoverIndex) {
    BeginBatchDraw();
    draw_background();
    draw_dim_overlay(100);
    
    settextstyle(50, 0, _T("SimHei"));
    settextcolor(WHITE);
    outtextxy((g_windowWidth - textwidth(_T("设置 (Settings)")))/2, 50, _T("设置 (Settings)"));
    
    for (int i = 0; i < buttonCount; i++) {
        if (i == 4) {
            // 背景不透明度滑块
            draw_opacity_slider(&buttons[i], g_settings.bgOpacity);
        } 
        else if (i == 5) {
            // 音乐音量滑块 (新增)
            draw_volume_slider(&buttons[i], g_settings.musicVolume);
        }
        else {
            draw_button(&buttons[i], i == hoverIndex);
        }
    }
    
    draw_music_ticker();
    
    EndBatchDraw();
}

void draw_leaderboard(Button* buttons, int buttonCount, int hoverIndex) {
    BeginBatchDraw();
    draw_background();
    draw_dim_overlay(100);
    
    settextstyle(50, 0, _T("SimHei"));
    settextcolor(WHITE);
    outtextxy((g_windowWidth - textwidth(_T("排行榜 (Leaderboard)")))/2, 50, _T("排行榜 (Leaderboard)"));
    
    settextstyle(24, 0, _T("SimHei"));
    int startY = 150;
    
    if (g_leaderboard.count == 0) {
        const TCHAR* msg = _T("暂无记录");
        outtextxy((g_windowWidth - textwidth(msg))/2, startY, msg);
    } else {
        for (int i = 0; i < g_leaderboard.count; i++) {
            TCHAR buf[128];
            TCHAR wName[32];
            #ifdef UNICODE
                mbstowcs(wName, g_leaderboard.entries[i].name, 32);
            #else
                strcpy(wName, g_leaderboard.entries[i].name);
            #endif
            
            _stprintf_s(buf, 128, _T("%d. %s - %d"), i+1, wName, g_leaderboard.entries[i].score);
            outtextxy((g_windowWidth - textwidth(buf))/2, startY + i * 40, buf);
        }
    }
    
    for (int i = 0; i < buttonCount; i++) {
        draw_button(&buttons[i], i == hoverIndex);
    }
    
    draw_music_ticker();
    
    EndBatchDraw();
}

void draw_game_over(PlayerState *p1, PlayerState *p2, GameMode mode) {
    int cx = g_windowWidth / 2;
    int cy = g_windowHeight / 2;
    
    // 覆盖框
    setfillcolor(BLACK);
    fillrectangle(cx - 200, cy - 100, cx + 200, cy + 100);
    setlinecolor(WHITE);
    rectangle(cx - 200, cy - 100, cx + 200, cy + 100);
    
    settextstyle(40, 0, _T("SimHei"));
    settextcolor(WHITE);
    
    if (mode == MODE_SINGLE || mode == MODE_LEVEL) {
        const TCHAR* msg = _T("GAME OVER");
        outtextxy(cx - textwidth(msg)/2, cy - 20, msg);
    } else {
        const TCHAR* msg;
        if (p1->isWinner && !p2->isWinner) {
            msg = _T("Player 1 Wins!");
        } else if (p2->isWinner && !p1->isWinner) {
            msg = _T("Player 2 Wins!");
        } else {
            msg = _T("平局 (Draw)!");
        }
        outtextxy(cx - textwidth(msg)/2, cy - 20, msg);
    }
    
    settextstyle(20, 0, _T("SimHei"));
    const TCHAR* sub = _T("按 ESC 返回主菜单 / ENTER 重新开始");
    outtextxy(cx - textwidth(sub)/2, cy + 50, sub);
    
    FlushBatchDraw();
}

bool is_mouse_over(const Button* btn, int mx, int my) {
    return mx >= btn->x && mx <= btn->x + btn->w &&
           my >= btn->y && my <= btn->y + btn->h;
}
