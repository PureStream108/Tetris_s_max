#include "../include/scene_story.h"
#include "../include/renderer.h"

static Button btnBack = {20, 20, 100, 40, _T("返回")};

static LevelID selectedStoryLevel = LEVEL_1;
static int textScrollY = 0;
static int listScrollY = 0;
static int lastMx = 0;
static int lastMy = 0;

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
    BeginBatchDraw();
    // 1. 绘制背景 (使用 solidrectangle 替代 cleardevice 以减少闪烁)
    setfillcolor(RGB(20, 20, 20));
    solidrectangle(0, 0, g_windowWidth, g_windowHeight);

    // 2. 绘制左侧列表背景
    setfillcolor(RGB(40, 40, 40));
    solidrectangle(0, 0, 300, g_windowHeight);

    // 3. 绘制返回按钮
    draw_button(&btnBack, false);

    // 4. 绘制关卡列表
    int startY = 80 + listScrollY;
    int itemHeight = 50;

    settextstyle(20, 0, _T("微软雅黑"));
    setbkmode(TRANSPARENT);

    int displayIndex = 0; // 用于计算显示位置的索引，跳过 OS 关卡
    for (int i = 0; i < LEVEL_COUNT; i++) {
        // 跳过 OS 系列关卡
        if (i >= LEVEL_OS_1 && i <= LEVEL_OS_2) continue;

        int y = startY + displayIndex * itemHeight;
        displayIndex++;
        
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
            // 直接显示未解锁，不需要鼠标悬浮
            settextcolor(RGB(255, 100, 100)); // 红色提示
            outtextxy(150, y + 15, _T("未解锁"));
        }
    }

    // 5. 绘制右侧剧情内容
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
        // 修改：改回微软雅黑
        settextstyle(24, 0, _T("微软雅黑"));
        settextcolor(WHITE);
        
        TCHAR wStory[4096];
        #ifdef UNICODE
            _tcscpy(wStory, level->story);
        #else
            strcpy(wStory, level->story);
        #endif

        // 修改：按段落绘制，增加间距
        int currentY = contentY + 60;
        int paragraphGap = 20; // 段落间距
        
        TCHAR* context = NULL;
        // 使用 _tcstok_s 分割段落 (注意：会修改 wStory，所以上面是拷贝了一份)
        TCHAR* line = _tcstok_s(wStory, _T("\n"), &context);
        
        while (line) {
            RECT r = {contentX, currentY, g_windowWidth - 40, currentY + 5000};
            
            // 计算高度
            // DT_CALCRECT: 计算矩形大小但不绘制
            drawtext(line, &r, DT_LEFT | DT_WORDBREAK | DT_CALCRECT);
            
            int h = r.bottom - r.top;
            
            // 绘制
            r.right = g_windowWidth - 40; // 恢复宽度
            r.bottom = currentY + h;
            drawtext(line, &r, DT_LEFT | DT_WORDBREAK);
            
            currentY += h + paragraphGap;
            
            line = _tcstok_s(NULL, _T("\n"), &context);
        }

    } else {
        settextstyle(24, 0, _T("微软雅黑"));
        settextcolor(LIGHTGRAY);
        outtextxy(contentX, contentY + 60, _T("该关卡剧情尚未解锁..."));
    }
    
    EndBatchDraw();
}

extern GameState g_gameState;

// 辅助：处理按钮点击
static bool is_button_clicked(Button *btn, int mx, int my) {
    return (mx >= btn->x && mx <= btn->x + btn->w && my >= btn->y && my <= btn->y + btn->h);
}

void handle_story_input(ExMessage *msg) {
    // 更新鼠标位置记录
    if (msg->message == WM_MOUSEMOVE) {
        lastMx = msg->x;
        lastMy = msg->y;
    }

    if (msg->message == WM_LBUTTONDOWN) {
        if (is_button_clicked(&btnBack, msg->x, msg->y)) {
            // 返回上一级 (选关页面)
            g_gameState = STATE_LEVEL_SELECT;
            return;
        }

        // 检查点击列表
        if (msg->x < 300 && msg->y > 80) {
            int clickedIndex = (msg->y - 80 - listScrollY) / 50;
            
            // 需要将点击的视觉索引映射回真实的 LevelID
            int currentVisualIndex = 0;
            int targetLevelIndex = -1;
            
            for (int i = 0; i < LEVEL_COUNT; i++) {
                if (i >= LEVEL_OS_1 && i <= LEVEL_OS_2) continue;
                
                if (currentVisualIndex == clickedIndex) {
                    targetLevelIndex = i;
                    break;
                }
                currentVisualIndex++;
            }
            
            if (targetLevelIndex != -1) {
                // 允许点击未解锁的，只是显示未解锁信息
                selectedStoryLevel = (LevelID)targetLevelIndex;
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
            // 计算实际显示的条目数
            int visibleCount = LEVEL_COUNT - 2; // 减去 2 个 OS 关卡
            int minScroll = g_windowHeight - (visibleCount * 50 + 80);
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
