#include "../include/scene_level_select.h"
#include "../include/renderer.h" // for draw_button, etc.
#include "../include/game_logic.h" // for start_level
#include <stdio.h>

// 地图偏移量
static int mapOffsetX = 0;
static int mapOffsetY = 0;

// 拖拽状态
static bool isDragging = false;
static int dragStartX = 0;
static int dragStartY = 0;
static int startMapOffsetX = 0;
static int startMapOffsetY = 0;

// 确认弹窗状态
static bool showConfirmDialog = false;
static LevelID selectedLevelID = LEVEL_1;
static int lastMx = 0;
static int lastMy = 0;

// 按钮定义
static Button btnBack = {20, 20, 100, 40, _T("返回")};
static Button btnStory = {20, 600, 100, 40, _T("剧情")}; // Y will be adjusted dynamically
static Button btnReset = {140, 600, 100, 40, _T("重置进度")}; // 新增
static Button btnUnlockAll = {260, 600, 100, 40, _T("剧情模式")}; // 新增

// 确认弹窗按钮
static Button btnConfirmYes;
static Button btnConfirmNo;

void init_level_select() {
    mapOffsetX = 0;
    mapOffsetY = 0;
    showConfirmDialog = false;
    isDragging = false;
    
    // 初始化按钮位置 (根据当前窗口大小)
    btnStory.y = g_windowHeight - 60;
    btnReset.y = g_windowHeight - 60;
    btnUnlockAll.y = g_windowHeight - 60;
}

void update_level_select() {
    // 可以在这里处理平滑滚动动画等
}

void draw_level_select() {
    draw_background();
    draw_dim_overlay(100);

    // 绘制连接线 (先画线，在方块下面)
    setlinestyle(PS_SOLID, 3);
    for (int i = 0; i < LEVEL_COUNT - 1; i++) {
        // 断开 PM-EX-3 (Index 12) 和 PM-OS-1 (Index 13) 之间的连线
        if (i == LEVEL_EX_3) continue;
        
        // 断开 PM-OS-2 和下一个（不存在的）之间的连线 (防止溢出或错误连线)
        if (i == LEVEL_OS_2) continue;

        int x1 = g_levels[i].mapX + mapOffsetX;
        int y1 = g_levels[i].mapY + mapOffsetY;
        int x2 = g_levels[i+1].mapX + mapOffsetX;
        int y2 = g_levels[i+1].mapY + mapOffsetY;
        
        // 节点中心
        int w = 120, h = 60;
        x1 += w/2; y1 += h/2;
        x2 += w/2; y2 += h/2;

        if (g_levels[i].isCompleted) {
            setlinecolor(GREEN);
        } else if (g_levels[i].isUnlocked) {
            setlinecolor(YELLOW);
        } else {
            setlinecolor(LIGHTGRAY);
        }
        
        line(x1, y1, x2, y2);
    }

    // 绘制关卡节点
    for (int i = 0; i < LEVEL_COUNT; i++) {
        int x = g_levels[i].mapX + mapOffsetX;
        int y = g_levels[i].mapY + mapOffsetY;
        int w = 120;
        int h = 60;

        // 屏幕外不绘制 (简单的剔除)
        if (x + w < 0 || x > g_windowWidth || y + h < 0 || y > g_windowHeight) continue;

        COLORREF fillColor;
        COLORREF borderColor;
        
        if (g_levels[i].isCompleted) {
            fillColor = RGB(0, 100, 0); // 深绿
            borderColor = GREEN;
        } else if (g_levels[i].isUnlocked) {
            fillColor = RGB(100, 100, 0); // 深黄
            borderColor = YELLOW;
        } else {
            fillColor = RGB(50, 50, 50); // 灰
            borderColor = LIGHTGRAY;
        }

        setfillcolor(fillColor);
        setlinecolor(borderColor);
        setlinestyle(PS_SOLID, 2);
        fillroundrect(x, y, x + w, y + h, 10, 10);

        // 绘制文字
        settextcolor(WHITE);
        setbkmode(TRANSPARENT);
        settextstyle(20, 0, _T("微软雅黑"));
        
        // 将 char* 转换为 TCHAR*
        TCHAR wName[32];
        #ifdef UNICODE
            _tcscpy(wName, g_levels[i].name);
        #else
            strcpy(wName, g_levels[i].name);
        #endif
        
        // 居中显示
        int textW = textwidth(wName);
        int textH = textheight(wName);
        outtextxy(x + (w - textW) / 2, y + (h - textH) / 2, wName);
    }

    // 绘制UI按钮 (固定在屏幕上)
    draw_button(&btnBack, false);
    
    // 更新剧情按钮位置
    btnStory.y = g_windowHeight - 60;
    btnReset.y = g_windowHeight - 60;
    btnUnlockAll.y = g_windowHeight - 60;
    
    draw_button(&btnStory, false);
    draw_button(&btnReset, false);
    draw_button(&btnUnlockAll, false);

    // 绘制确认弹窗
    if (showConfirmDialog) {
        // 半透明遮罩
        setfillcolor(RGB(0, 0, 0));
        // 既然之前实现了 dim overlay，这里直接画个框吧
        
        int boxW = 400;
        int boxH = 200;
        int boxX = (g_windowWidth - boxW) / 2;
        int boxY = (g_windowHeight - boxH) / 2;

        setfillcolor(RGB(30, 30, 30));
        setlinecolor(WHITE);
        setlinestyle(PS_SOLID, 2);
        fillroundrect(boxX, boxY, boxX + boxW, boxY + boxH, 20, 20);

        settextcolor(WHITE);
        settextstyle(24, 0, _T("微软雅黑"));
        const TCHAR* title = _T("是否进入游戏？");
        outtextxy(boxX + (boxW - textwidth(title)) / 2, boxY + 40, title);
        
        // 关卡名
        TCHAR wTitle[64];
        #ifdef UNICODE
            _tcscpy(wTitle, g_levels[selectedLevelID].title);
        #else
            strcpy(wTitle, g_levels[selectedLevelID].title);
        #endif
        settextstyle(18, 0, _T("微软雅黑"));
        settextcolor(YELLOW);
        outtextxy(boxX + (boxW - textwidth(wTitle)) / 2, boxY + 80, wTitle);

        // 按钮
        btnConfirmYes.x = boxX + 50;
        btnConfirmYes.y = boxY + 130;
        btnConfirmYes.w = 100;
        btnConfirmYes.h = 40;
        btnConfirmYes.text = _T("进入");

        btnConfirmNo.x = boxX + 250;
        btnConfirmNo.y = boxY + 130;
        btnConfirmNo.w = 100;
        btnConfirmNo.h = 40;
        btnConfirmNo.text = _T("取消");

        draw_button(&btnConfirmYes, false);
        draw_button(&btnConfirmNo, false);
    }
    
    FlushBatchDraw();
}

// 辅助函数：检查点是否在矩形内
static bool is_point_in_rect(int x, int y, int rx, int ry, int rw, int rh) {
    return (x >= rx && x <= rx + rw && y >= ry && y <= ry + rh);
}

// 辅助函数：处理按钮点击
static bool is_button_clicked(Button *btn, int mx, int my) {
    return is_point_in_rect(mx, my, btn->x, btn->y, btn->w, btn->h);
}

extern GameState g_gameState;
extern PlayerState g_player1; // 需要访问 g_player1 来设置状态
extern void save_settings();

void handle_level_select_input(ExMessage *msg) {
    // 始终更新鼠标位置
    if (msg->message == WM_MOUSEMOVE) {
        lastMx = msg->x;
        lastMy = msg->y;
    }

    if (msg->message == WM_LBUTTONDOWN) {
        if (showConfirmDialog) {
            if (is_button_clicked(&btnConfirmYes, msg->x, msg->y)) {
                // 进入游戏
                showConfirmDialog = false;
                g_gameState = STATE_GAME_LEVEL;
                
                // 使用 start_level 统一初始化 (包含 restrictPieces 处理)
                start_level(&g_player1, 1, selectedLevelID);
                
            } else if (is_button_clicked(&btnConfirmNo, msg->x, msg->y)) {
                showConfirmDialog = false;
            }
            return; // 弹窗开启时屏蔽其他点击
        }

        // UI 按钮
        if (is_button_clicked(&btnBack, msg->x, msg->y)) {
            g_gameState = STATE_MENU;
            return;
        }
        if (is_button_clicked(&btnStory, msg->x, msg->y)) {
            g_gameState = STATE_STORY;
            return;
        }
        
        // 重置进度
        if (is_button_clicked(&btnReset, msg->x, msg->y)) {
            g_settings.maxUnlockedLevel = 0;
            // 不重置剧情进度 (g_settings.maxUnlockedStoryLevel 保持不变)
            
            // 更新关卡状态
            for (int i = 0; i < LEVEL_COUNT; i++) {
                g_levels[i].isUnlocked = (i == 0);
                g_levels[i].isCompleted = false;
            }
            save_settings();
            return;
        }

        // 剧情模式 (一键解锁)
        if (is_button_clicked(&btnUnlockAll, msg->x, msg->y)) {
            g_settings.maxUnlockedLevel = LEVEL_COUNT;
            g_settings.maxUnlockedStoryLevel = LEVEL_COUNT;
            
            // 更新关卡状态
            for (int i = 0; i < LEVEL_COUNT; i++) {
                g_levels[i].isUnlocked = true;
            }
            save_settings();
            return;
        }

        // 检查点击关卡节点
        for (int i = 0; i < LEVEL_COUNT; i++) {
            int x = g_levels[i].mapX + mapOffsetX;
            int y = g_levels[i].mapY + mapOffsetY;
            int w = 120;
            int h = 60;

            if (is_point_in_rect(msg->x, msg->y, x, y, w, h)) {
                if (g_levels[i].isUnlocked) {
                    selectedLevelID = (LevelID)i;
                    showConfirmDialog = true;
                }
                // 可以在这里播放音效或提示未解锁
                return;
            }
        }

        // 开始拖拽
        isDragging = true;
        dragStartX = msg->x;
        dragStartY = msg->y;
        startMapOffsetX = mapOffsetX;
        startMapOffsetY = mapOffsetY;

    } else if (msg->message == WM_LBUTTONUP) {
        isDragging = false;
    } else if (msg->message == WM_MOUSEMOVE) {
        if (isDragging) {
            int dx = msg->x - dragStartX;
            int dy = msg->y - dragStartY;
            mapOffsetX = startMapOffsetX + dx;
            mapOffsetY = startMapOffsetY + dy;
        }
    }
}
