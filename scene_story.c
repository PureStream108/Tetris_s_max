#include "../include/scene_story.h"
#include "../include/renderer.h"

static Button btnBack = {20, 20, 100, 40, _T("返回")};

static LevelID selectedStoryLevel = LEVEL_1;
static int textScrollY = 0;
static int listScrollY = 0;

void init_story_scene() {
    textScrollY = 0;
    listScrollY = 0;
    // 默认选中第一个已解锁的关卡 (使用 maxUnlockedStoryLevel 判断)
    for (int i = 0; i < LEVEL_COUNT; i++) {
        if (i <= g_settings.maxUnlockedStoryLevel) {
            selectedStoryLevel = (LevelID)i;
            break;
        }
    }
}

void draw_story_scene() {
    setbkcolor(RGB(20, 20, 20));
    cleardevice();

    // 1. 绘制左侧列表背景
    setfillcolor(RGB(40, 40, 40));
    solidrectangle(0, 0, 300, g_windowHeight);

    // 2. 绘制返回按钮
    draw_button(&btnBack, false);

    // 3. 绘制关卡列表
    int startY = 80 + listScrollY;
    int itemHeight = 50;

    settextstyle(20, 0, _T("微软雅黑"));
    setbkmode(TRANSPARENT);

    for (int i = 0; i < LEVEL_COUNT; i++) {
        int y = startY + i * itemHeight;
        
        // 简单的裁剪
        if (y + itemHeight < 0) continue;
        if (y > g_windowHeight) break;

        // 选中高亮
        if (i == selectedStoryLevel) {
            setfillcolor(RGB(80, 80, 80));
            solidrectangle(0, y, 300, y + itemHeight);
        }

        bool isStoryUnlocked = (i <= g_settings.maxUnlockedStoryLevel);

        if (isStoryUnlocked) {
            settextcolor(WHITE);
        } else {
            settextcolor(LIGHTGRAY);
        }

        TCHAR wName[32];
        #ifdef UNICODE
            _tcscpy(wName, g_levels[i].name);
        #else
            strcpy(wName, g_levels[i].name);
        #endif
        
        outtextxy(20, y + 15, wName);
        
        if (!isStoryUnlocked) {
            outtextxy(150, y + 15, _T("(未解锁)"));
        }
    }

    // 4. 绘制右侧剧情内容
    int contentX = 320;
    int contentY = 100 + textScrollY;
    
    LevelData *level = &g_levels[selectedStoryLevel];
    
    // 标题
    settextstyle(30, 0, _T("微软雅黑"));
    settextcolor(YELLOW);
    TCHAR wTitle[64];
    #ifdef UNICODE
        _tcscpy(wTitle, level->title);
    #else
        strcpy(wTitle, level->title);
    #endif
    outtextxy(contentX, contentY, wTitle);

    // 内容 (简单的多行文本绘制)
    bool isStoryUnlocked = (selectedStoryLevel <= g_settings.maxUnlockedStoryLevel);
    if (isStoryUnlocked) {
        settextstyle(24, 0, _T("微软雅黑"));
        settextcolor(WHITE);
        
        TCHAR wStory[1024];
        #ifdef UNICODE
            _tcscpy(wStory, level->story);
        #else
            strcpy(wStory, level->story);
        #endif

        RECT r = {contentX, contentY + 60, g_windowWidth - 20, contentY + 1000};
        drawtext(wStory, &r, DT_LEFT | DT_WORDBREAK);
    } else {
        settextstyle(24, 0, _T("微软雅黑"));
        settextcolor(LIGHTGRAY);
        outtextxy(contentX, contentY + 60, _T("该关卡剧情尚未解锁..."));
    }
    
    FlushBatchDraw();
}

extern GameState g_gameState;

// 辅助：处理按钮点击
static bool is_button_clicked(Button *btn, int mx, int my) {
    return (mx >= btn->x && mx <= btn->x + btn->w && my >= btn->y && my <= btn->y + btn->h);
}

void handle_story_input(ExMessage *msg) {
    if (msg->message == WM_LBUTTONDOWN) {
        if (is_button_clicked(&btnBack, msg->x, msg->y)) {
            // 返回上一级 (选关页面)
            g_gameState = STATE_LEVEL_SELECT;
            return;
        }

        // 检查点击列表
        if (msg->x < 300 && msg->y > 80) {
            int index = (msg->y - 80 - listScrollY) / 50;
            if (index >= 0 && index < LEVEL_COUNT) {
                // 允许点击未解锁的，只是显示未解锁信息
                selectedStoryLevel = (LevelID)index;
                textScrollY = 0; // 重置文本滚动
            }
        }
    } else if (msg->message == WM_MOUSEWHEEL) {
        // 滚轮处理
        if (msg->x < 300) {
            // 左侧滚动
            listScrollY += msg->wheel / 120 * 30;
            if (listScrollY > 0) listScrollY = 0;
            // 限制底部 (简单处理)
            int minScroll = g_windowHeight - (LEVEL_COUNT * 50 + 80);
            if (minScroll > 0) minScroll = 0;
            if (listScrollY < minScroll) listScrollY = minScroll;
        } else {
            // 右侧滚动
            textScrollY += msg->wheel / 120 * 30;
            if (textScrollY > 0) textScrollY = 0;
            // 限制底部? 暂时不严格限制
        }
    }
}
