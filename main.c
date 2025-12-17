#include "include/common.h"
#include "include/renderer.h"
#include "include/game_logic.h"
#include "include/input.h"
#include "include/tetromino.h"
#include "include/levels.h"
#include "include/scene_level_select.h"
#include "include/scene_story.h"
#include <mmsystem.h> 
#include <commdlg.h> // 文件对话框支持

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "comdlg32.lib")

// 全局设置实例
GameSettings g_settings = {
    true,           // musicEnabled
    DIFF_NORMAL,    // difficulty
    _T("bgm.mp3"),  // musicPath
    _T(""),         // bgImagePath
    43,             // bgOpacity (默认 26%)
    150,            // musicVolume (默认 150 = 15%)
    0,              // maxUnlockedLevel
    BASE_WINDOW_WIDTH, // windowWidth (默认)
    BASE_WINDOW_HEIGHT // windowHeight (默认)
};

// 全局变量：双人模式时间限制 (默认 60秒)
int g_dualTimeLimit = 60;

// 全局游戏状态 (供其他模块访问)
GameState g_gameState = STATE_MENU;
PlayerState g_player1;
PlayerState g_player2; // 仅用于双人

// 按钮定义
#define BTN_MENU_COUNT 6
Button menuButtons[BTN_MENU_COUNT] = {
    {0, 300, 300, 50, _T("单人游戏 (1 Player)")},
    {0, 370, 300, 50, _T("双人对战 (2 Players)")},
    {0, 440, 300, 50, _T("关卡模式 (Level Mode)")},
    {0, 510, 300, 50, _T("排行榜 (Leaderboard)")},
    {0, 580, 300, 50, _T("设置 (Settings)")},
    {0, 650, 300, 50, _T("退出 (Exit)")}
};

#define BTN_SETTINGS_COUNT 7
Button settingsButtons[BTN_SETTINGS_COUNT] = {
    {0, 200, 400, 50, _T("Music: ON")},
    {0, 260, 400, 50, _T("设置默认音乐 (From Musics/)")},
    {0, 320, 400, 50, _T("设置默认背景 (From Images/)")},
    {0, 380, 400, 50, _T("难度 (Difficulty): Normal")},
    {0, 440, 400, 50, _T("背景不透明度 (Opacity)")}, 
    {0, 500, 400, 50, _T("音乐音量 (Volume)")},     
    {0, 610, 400, 50, _T("返回 (Back)")}            
};

#define BTN_LEADERBOARD_COUNT 1
Button leaderboardButtons[BTN_LEADERBOARD_COUNT] = {
    {0, 550, 300, 50, _T("返回 (Back)")}
};

#define BTN_TIME_SELECT_COUNT 5
Button timeSelectButtons[BTN_TIME_SELECT_COUNT] = {
    {0, 200, 300, 50, _T("1 分钟 (1 Minute)")},
    {0, 270, 300, 50, _T("5 分钟 (5 Minutes)")},
    {0, 340, 300, 50, _T("10 分钟 (10 Minutes)")},
    {0, 410, 300, 50, _T("不限时 (Unlimited)")},
    {0, 500, 300, 50, _T("返回 (Back)")}
};

#define BTN_PAUSE_COUNT 3
Button pauseButtons[BTN_PAUSE_COUNT] = {
    {0, 0, 200, 50, _T("继续游戏")},
    {0, 0, 200, 50, _T("重新开始")},
    {0, 0, 200, 50, _T("主菜单")}
};

// 居中按钮辅助函数 (更新：支持垂直自适应)
void center_buttons(Button* btns, int count) {
    // 基础垂直起始位置（相对于 700 高度窗口的比例）
    // 假设菜单大约占据屏幕中间 50% 区域
    int totalHeight = count * 60; // 50(height) + 10(gap)
    int startY = (g_windowHeight - totalHeight) / 2 + 50; // +50 偏移以避开标题
    
    for (int i = 0; i < count; i++) {
        btns[i].x = (g_windowWidth - btns[i].w) / 2;
        btns[i].y = startY + i * 70; // 70 = 50 height + 20 gap
    }
}

// 暂停菜单居中 (垂直居中于屏幕中心)
void center_pause_buttons() {
    int totalHeight = BTN_PAUSE_COUNT * 60;
    int startY = (g_windowHeight - totalHeight) / 2;
    for (int i = 0; i < BTN_PAUSE_COUNT; i++) {
        pauseButtons[i].x = (g_windowWidth - pauseButtons[i].w) / 2;
        pauseButtons[i].y = startY + i * 70;
    }
}

// 辅助函数：从路径中提取文件名（不带扩展名）
void extract_filename(const TCHAR* path, TCHAR* outName, int maxLen) {
    const TCHAR* lastSlash = _tcsrchr(path, '\\');
    if (!lastSlash) lastSlash = _tcsrchr(path, '/');
    
    const TCHAR* nameStart = lastSlash ? (lastSlash + 1) : path;
    
    _tcsncpy(outName, nameStart, maxLen - 1);
    outName[maxLen - 1] = '\0';
    
    // 去掉扩展名
    TCHAR* lastDot = _tcsrchr(outName, '.');
    if (lastDot) *lastDot = '\0';
}

// 更新音乐播放状态
void update_music() {
    if (g_settings.musicEnabled) {
        // 先尝试关闭，忽略错误
        mciSendString(_T("close BGM"), NULL, 0, NULL);
        
        // 尝试方法 1: 指定 mpegvideo 类型 (适用于大多数 MP3)
        TCHAR cmd[MAX_PATH_LEN + 64];
        _stprintf_s(cmd, MAX_PATH_LEN + 64, _T("open \"%s\" type mpegvideo alias BGM"), g_settings.musicPath);
        
        MCIERROR err = mciSendString(cmd, NULL, 0, NULL);
        
        // 尝试方法 2: 自动检测类型 (如果方法1失败)
        if (err != 0) {
             // 尝试使用短路径以避免特殊字符问题
             TCHAR shortPath[MAX_PATH_LEN];
             GetShortPathName(g_settings.musicPath, shortPath, MAX_PATH_LEN);
             
             _stprintf_s(cmd, MAX_PATH_LEN + 64, _T("open \"%s\" alias BGM"), shortPath);
             err = mciSendString(cmd, NULL, 0, NULL);
        }
        
        if (err == 0) {
            mciSendString(_T("play BGM repeat"), NULL, 0, NULL);
            // 应用音量
            TCHAR volCmd[64];
            _stprintf_s(volCmd, 64, _T("setaudio BGM volume to %d"), g_settings.musicVolume);
            mciSendString(volCmd, NULL, 0, NULL);
            
            // 提取歌名用于滚动显示
            extract_filename(g_settings.musicPath, g_currentMusicTitle, 256);
        } else {
            // 播放失败，清空歌名或显示错误
            _tcscpy(g_currentMusicTitle, _T("播放失败"));
        }
    } else {
        mciSendString(_T("stop BGM"), NULL, 0, NULL);
        _tcscpy(g_currentMusicTitle, _T("")); // 清空标题
    }
}

// 根据设置更新按钮文本
void update_settings_buttons() {
    // 音乐开关
    settingsButtons[0].text = g_settings.musicEnabled ? _T("音乐 (Music): ON") : _T("音乐 (Music): OFF");
    
    // 难度
    switch(g_settings.difficulty) {
        case DIFF_EASY: settingsButtons[3].text = _T("难度 (Difficulty): Easy"); break;
        case DIFF_NORMAL: settingsButtons[3].text = _T("难度 (Difficulty): Normal"); break;
        case DIFF_HARD: settingsButtons[3].text = _T("难度 (Difficulty): Hard"); break;
    }
    
    // Index 4 是滑块，由 renderer.c 直接绘制数值，此处不需要更新文本
}

// 打开文件对话框
bool open_file_dialog(TCHAR* buffer, const TCHAR* filter) {
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetHWnd();
    ofn.lpstrFile = buffer;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = MAX_PATH_LEN;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    return GetOpenFileName(&ofn) == TRUE;
}

// --- 设置加载与保存 ---

void load_settings() {
    FILE* f = fopen("settings.cfg", "rb");
    if (f) {
        fread(&g_settings, sizeof(GameSettings), 1, f);
        fclose(f);
        
        // [FIX] 强制修正旧的默认值 (1000/100 -> 150/26)
        // 扩大判断范围：因为用户反馈显示 99% (即 ~990)，说明可能并非严格等于 1000
        // 只要检测到音量过大且不透明度过高，就认为是旧配置，强制重置
        if (g_settings.musicVolume >= 900 && g_settings.bgOpacity >= 90) {
            g_settings.musicVolume = 150;
            g_settings.bgOpacity = 26;
            // 立即保存修正后的配置，防止下次启动再次判断失效（虽然这里逻辑是幂等的）
            // 但保存一下更安全
            save_settings();
        }
        
        // 验证加载的窗口尺寸是否合法，防止异常值
        if (g_settings.windowWidth < 400 || g_settings.windowWidth > 3000) 
            g_settings.windowWidth = BASE_WINDOW_WIDTH;
        if (g_settings.windowHeight < 300 || g_settings.windowHeight > 2000) 
            g_settings.windowHeight = BASE_WINDOW_HEIGHT;
            
        // 应用加载的窗口尺寸到全局变量
        g_windowWidth = g_settings.windowWidth;
        g_windowHeight = g_settings.windowHeight;
        
        // 重新计算方块大小
        g_blockSize = (g_windowHeight - 100) / BOARD_ROWS;
        if (g_blockSize < 15) g_blockSize = 15;
    }
}

// 扫描 images 文件夹并设置第一张图片为背景
void scan_images_folder() {
    WIN32_FIND_DATA findData;
    HANDLE hFind;
    
    // 搜索 images 文件夹下的所有文件
    // 注意：工作目录已经设为 exe 所在目录
    hFind = FindFirstFile(_T("images\\*.*"), &findData);
    
    if (hFind == INVALID_HANDLE_VALUE) return;
    
    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            // 检查扩展名
            TCHAR* ext = _tcsrchr(findData.cFileName, '.');
            if (ext) {
                if (_tcsicmp(ext, _T(".jpg")) == 0 || 
                    _tcsicmp(ext, _T(".png")) == 0 || 
                    _tcsicmp(ext, _T(".bmp")) == 0) {
                    
                    // 找到第一张图片，构造相对路径
                    TCHAR path[MAX_PATH_LEN];
                    _stprintf_s(path, MAX_PATH_LEN, _T("images\\%s"), findData.cFileName);
                    
                    // 更新设置
                    _tcscpy(g_settings.bgImagePath, path);
                    reload_background();
                    
                    // 找到一张就退出
                    FindClose(hFind);
                    return;
                }
            }
        }
    } while (FindNextFile(hFind, &findData));
    
    FindClose(hFind);
}

// 扫描 musics 文件夹并设置第一首音乐
void scan_music_folder() {
    WIN32_FIND_DATA findData;
    HANDLE hFind;
    
    // 搜索 musics 文件夹下的所有文件
    hFind = FindFirstFile(_T("musics\\*.*"), &findData);
    
    if (hFind == INVALID_HANDLE_VALUE) return;
    
    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            // 检查扩展名
            TCHAR* ext = _tcsrchr(findData.cFileName, '.');
            if (ext) {
                if (_tcsicmp(ext, _T(".mp3")) == 0 || 
                    _tcsicmp(ext, _T(".wav")) == 0 || 
                    _tcsicmp(ext, _T(".mid")) == 0) {
                    
                    // 找到第一首音乐，构造相对路径
                    TCHAR path[MAX_PATH_LEN];
                    _stprintf_s(path, MAX_PATH_LEN, _T("musics\\%s"), findData.cFileName);
                    
                    // 更新设置
                    _tcscpy(g_settings.musicPath, path);
                    update_music(); // 立即应用音乐
                    
                    // 找到一首就退出
                    FindClose(hFind);
                    return;
                }
            }
        }
    } while (FindNextFile(hFind, &findData));
    
    FindClose(hFind);
}

// 扫描 icons 文件夹并设置窗口图标
void scan_icon_folder() {
    WIN32_FIND_DATA findData;
    HANDLE hFind;
    
    // 搜索 icons 文件夹下的所有文件
    hFind = FindFirstFile(_T("icons\\*.*"), &findData);
    
    if (hFind == INVALID_HANDLE_VALUE) return;
    
    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            // 检查扩展名 (支持 .ico, .png, .jpg)
            // 注意：LoadImage 最好用 .ico，但我们会尝试加载图片
            TCHAR* ext = _tcsrchr(findData.cFileName, '.');
            if (ext) {
                // 构造路径
                TCHAR path[MAX_PATH_LEN];
                _stprintf_s(path, MAX_PATH_LEN, _T("icons\\%s"), findData.cFileName);
                
                // 加载图标/图片
                // 使用 LoadImage 加载为 ICON
                // LR_LOADFROMFILE: 从文件加载
                // LR_DEFAULTSIZE: 使用默认图标大小
                HANDLE hIcon = LoadImage(NULL, path, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
                
                if (hIcon) {
                    // 获取当前窗口句柄
                    HWND hwnd = GetHWnd();
                    // 设置窗口大图标和小图标
                    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
                    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
                    
                    FindClose(hFind);
                    return;
                }
            }
        }
    } while (FindNextFile(hFind, &findData));
    
    FindClose(hFind);
}

void save_settings() {
    // 保存前更新当前窗口尺寸
    g_settings.windowWidth = g_windowWidth;
    g_settings.windowHeight = g_windowHeight;
    
    FILE* f = fopen("settings.cfg", "wb");
    if (f) {
        fwrite(&g_settings, sizeof(GameSettings), 1, f);
        fclose(f);
    }
}

// 设置菜单专用处理，增加滑块逻辑
bool handle_settings_mouse(Button* buttons, int count, int* hoverIndex) {
    MOUSEMSG msg;
    
    while (MouseHit()) {
        msg = GetMouseMsg();
        
        // 滑块索引
        int opacitySliderIdx = 4;
        int volumeSliderIdx = 5;
        
        if (msg.uMsg == WM_MOUSEMOVE) {
            *hoverIndex = -1;
            for (int i = 0; i < count; i++) {
                if (is_mouse_over(&buttons[i], msg.x, msg.y)) {
                    *hoverIndex = i;
                    break;
                }
            }
            
            // 拖动逻辑 - 不透明度
            if (msg.mkLButton && is_mouse_over(&buttons[opacitySliderIdx], msg.x, msg.y)) {
                int barX = buttons[opacitySliderIdx].x;
                int barW = buttons[opacitySliderIdx].w;
                int val = (msg.x - barX) * 100 / barW;
                if (val < 0) val = 0;
                if (val > 100) val = 100;
                g_settings.bgOpacity = val;
            }
            
            // 拖动逻辑 - 音量
            if (msg.mkLButton && is_mouse_over(&buttons[volumeSliderIdx], msg.x, msg.y)) {
                int barX = buttons[volumeSliderIdx].x;
                int barW = buttons[volumeSliderIdx].w;
                int val = (msg.x - barX) * 1000 / barW; // 映射到 0-1000
                if (val < 0) val = 0;
                if (val > 1000) val = 1000;
                g_settings.musicVolume = val;
                
                // 实时调整音量
                if (g_settings.musicEnabled) {
                     TCHAR volCmd[64];
                     _stprintf_s(volCmd, 64, _T("setaudio BGM volume to %d"), g_settings.musicVolume);
                     mciSendString(volCmd, NULL, 0, NULL);
                }
            }
        }
        else if (msg.uMsg == WM_LBUTTONDOWN) {
            // 点击直接跳转 - 不透明度
            if (is_mouse_over(&buttons[opacitySliderIdx], msg.x, msg.y)) {
                int barX = buttons[opacitySliderIdx].x;
                int barW = buttons[opacitySliderIdx].w;
                int val = (msg.x - barX) * 100 / barW;
                if (val < 0) val = 0;
                if (val > 100) val = 100;
                g_settings.bgOpacity = val;
            }
            // 点击直接跳转 - 音量
            else if (is_mouse_over(&buttons[volumeSliderIdx], msg.x, msg.y)) {
                int barX = buttons[volumeSliderIdx].x;
                int barW = buttons[volumeSliderIdx].w;
                int val = (msg.x - barX) * 1000 / barW;
                if (val < 0) val = 0;
                if (val > 1000) val = 1000;
                g_settings.musicVolume = val;
                
                if (g_settings.musicEnabled) {
                     TCHAR volCmd[64];
                     _stprintf_s(volCmd, 64, _T("setaudio BGM volume to %d"), g_settings.musicVolume);
                     mciSendString(volCmd, NULL, 0, NULL);
                }
            }
        }
        else if (msg.uMsg == WM_LBUTTONUP) {
            for (int i = 0; i < count; i++) {
                if (is_mouse_over(&buttons[i], msg.x, msg.y)) {
                    // 如果是滑块，不返回true（不触发按钮事件）
                    if (i == opacitySliderIdx || i == volumeSliderIdx) {
                        continue;
                    }
                    *hoverIndex = i;
                    return true;
                }
            }
        }
    }
    return false;
}

int main() {
    // [FIX] 设置当前工作目录为 exe 所在目录，彻底解决相对路径资源加载问题
    TCHAR exePath[MAX_PATH_LEN];
    if (GetModuleFileName(NULL, exePath, MAX_PATH_LEN)) {
        TCHAR* lastSlash = _tcsrchr(exePath, '\\');
        if (lastSlash) {
            *lastSlash = '\0'; // 去掉文件名，保留目录
            SetCurrentDirectory(exePath);
        }
    }

    // 初始化随机数种子
    srand((unsigned int)time(NULL));
    
    // 加载排行榜
    load_leaderboard();
    
    // 加载用户设置 (包括窗口大小)
    load_settings();

    // 校验 maxUnlockedLevel 和 窗口尺寸，防止旧存档导致的数据异常
    if (g_settings.maxUnlockedLevel < 0 || g_settings.maxUnlockedLevel >= LEVEL_COUNT) {
        g_settings.maxUnlockedLevel = 0; // 重置为 0
    }
    if (g_settings.maxUnlockedStoryLevel < 0 || g_settings.maxUnlockedStoryLevel >= LEVEL_COUNT) {
        g_settings.maxUnlockedStoryLevel = 0;
    }
    if (g_settings.windowWidth < 400 || g_settings.windowWidth > 3000) {
        g_settings.windowWidth = BASE_WINDOW_WIDTH;
    }
    if (g_settings.windowHeight < 300 || g_settings.windowHeight > 2000) {
        g_settings.windowHeight = BASE_WINDOW_HEIGHT;
    }

    // [NEW] 初始化关卡数据
    init_levels();
    // 恢复已解锁关卡状态
    for (int i = 0; i <= g_settings.maxUnlockedLevel; i++) {
        if (i < LEVEL_COUNT) g_levels[i].isUnlocked = true;
    }
    // 第一关永远解锁
    g_levels[0].isUnlocked = true;
    
    // 确保剧情解锁状态正确 (注意：这里我们暂时没有在 LevelData 里存 isStoryUnlocked，
    // 而是会在 scene_story.c 里直接用 maxUnlockedStoryLevel 判断)
    // 但为了逻辑一致性，如果以后加了 isStoryUnlocked，这里也要初始化
    
    // 自动扫描 images 文件夹并设置背景 (如果没有背景或为了强制默认)
    // 用户需求："每次游戏启动的时候只读images下的第一张图片"
    // 这意味着即使有保存的设置，也优先读取 images 文件夹
    scan_images_folder();
    
    // [NEW] 自动扫描 musics 文件夹并设置第一首音乐
    scan_music_folder();
    
    // 初始化图形窗口 (在 renderer.c 中会设置 WS_THICKFRAME)
    init_game_window();
    
    // [NEW] 初始化窗口后，立即设置图标
    scan_icon_folder();
    
    // [FIX] 显式调用 reload_background 确保在窗口初始化后应用已加载的背景图设置
    reload_background();
    
    // 初始化选关场景
    init_level_select();
    init_story_scene();
    
    center_buttons(menuButtons, BTN_MENU_COUNT);
    center_buttons(settingsButtons, BTN_SETTINGS_COUNT);
    center_buttons(leaderboardButtons, BTN_LEADERBOARD_COUNT);
    center_buttons(timeSelectButtons, BTN_TIME_SELECT_COUNT);
    center_pause_buttons(); // 初始化暂停菜单位置
    
    // 播放开场动画
    draw_intro();
    
    // 播放音乐
    update_music();

    // 状态机
    g_gameState = STATE_MENU;
    GameMode selectedMode = MODE_SINGLE;
    
    clock_t gameStartTime;
    bool quitApp = false;
    
    // 菜单按键状态跟踪
    bool enterHeld = false;
    bool escHeld = false;
    bool spaceHeld = false;
    
    // 暂停菜单交互
    int pauseHoverIdx = -1;
    
    HWND hwnd = GetHWnd(); // 获取窗口句柄用于检测大小
    
    // 窗口调整计时器，用于防抖
    static clock_t resize_timer = 0;

    // --- 主程序循环 ---
    while (!quitApp) {
        
        // --- 窗口大小动态检测 (增加防抖机制) ---
        RECT rect;
        if (GetClientRect(hwnd, &rect)) {
            int w = rect.right - rect.left;
            int h = rect.bottom - rect.top;
            
            if (w > 0 && h > 0 && (w != g_windowWidth || h != g_windowHeight)) {
                // 如果发现尺寸变化，重置计时器
                if (resize_timer == 0) {
                    resize_timer = clock();
                } else if (clock() - resize_timer > 300) { // 等待 300ms 稳定
                    // 只有在尺寸稳定后才执行更新
                    g_windowWidth = w;
                    g_windowHeight = h;
                    
                    // 动态计算方块大小 (基于高度适配)
                    // 预留约 100px 的上下边距
                    g_blockSize = (h - 100) / BOARD_ROWS;
                    if (g_blockSize < 15) g_blockSize = 15; // 最小限制
                    
                    // 重新初始化绘图缓冲
                    update_window_size();
                    
                    // 窗口重建后必须更新句柄
                    hwnd = GetHWnd();
                    
                    // 重新居中按钮
                    center_buttons(menuButtons, BTN_MENU_COUNT);
                    center_buttons(settingsButtons, BTN_SETTINGS_COUNT);
                    center_buttons(leaderboardButtons, BTN_LEADERBOARD_COUNT);
                    center_buttons(timeSelectButtons, BTN_TIME_SELECT_COUNT);
                    center_pause_buttons(); // 重新居中暂停菜单
                    
                    // 保存新设置 (包括窗口大小)
                    save_settings();
                    
                    resize_timer = 0; // 重置计时器
                }
            } else {
                resize_timer = 0; // 尺寸已匹配，无需计时
            }
        }

        // 根据状态分发处理
        if (g_gameState == STATE_MENU) {
            static int hoverIdx = -1;
            draw_menu(menuButtons, BTN_MENU_COUNT, hoverIdx);
            
            if (handle_mouse_menu(menuButtons, BTN_MENU_COUNT, &hoverIdx)) {
                switch (hoverIdx) {
                    case 0: selectedMode = MODE_SINGLE; g_gameState = STATE_GAME_SINGLE; break;
                    case 1: selectedMode = MODE_DUAL; g_gameState = STATE_TIME_SELECT; break; // 进入时间选择
                    case 2: selectedMode = MODE_LEVEL; g_gameState = STATE_LEVEL_SELECT; init_level_select(); break; // 进入选关
                    case 3: g_gameState = STATE_LEADERBOARD; break;
                    case 4: g_gameState = STATE_SETTINGS; break;
                    case 5: quitApp = true; break;
                }
                Sleep(200);
            }
            
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                 if (!escHeld) {
                     quitApp = true;
                     escHeld = true;
                 }
            } else escHeld = false;
            
            Sleep(10);
        }
        else if (g_gameState == STATE_LEVEL_SELECT) {
            ExMessage msg = {0};
            while (peekmessage(&msg, EX_MOUSE)) {
                handle_level_select_input(&msg);
            }
            // handle_level_select_input 可能修改 g_gameState (进入游戏或返回菜单)
            // 如果进入游戏，它已经调用了 init_player
            
            if (g_gameState == STATE_LEVEL_SELECT) { // 如果还在当前状态
                update_level_select();
                draw_level_select();
            }
            Sleep(10);
        }
        else if (g_gameState == STATE_STORY) {
            ExMessage msg = {0};
            while (peekmessage(&msg, EX_MOUSE | EX_KEY)) {
                 handle_story_input(&msg);
            }
            draw_story_scene();
            Sleep(10);
        }
        else if (g_gameState == STATE_TIME_SELECT) {
            static int hoverIdx = -1;
            // 使用临时函数名，稍后在 renderer.c 中实现
            extern void draw_time_select_menu(Button* buttons, int buttonCount, int hoverIndex);
            draw_time_select_menu(timeSelectButtons, BTN_TIME_SELECT_COUNT, hoverIdx);
            
            if (handle_mouse_menu(timeSelectButtons, BTN_TIME_SELECT_COUNT, &hoverIdx)) {
                switch (hoverIdx) {
                    case 0: g_dualTimeLimit = 60; g_gameState = STATE_GAME_DUAL; break;
                    case 1: g_dualTimeLimit = 300; g_gameState = STATE_GAME_DUAL; break;
                    case 2: g_dualTimeLimit = 600; g_gameState = STATE_GAME_DUAL; break;
                    case 3: g_dualTimeLimit = -1; g_gameState = STATE_GAME_DUAL; break;
                    case 4: g_gameState = STATE_MENU; break;
                }
                Sleep(200);
            }
            
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                 if (!escHeld) {
                     g_gameState = STATE_MENU;
                     escHeld = true;
                 }
            } else escHeld = false;
            
            Sleep(10);
        }
        else if (g_gameState == STATE_LEADERBOARD) {
            static int hoverIdx = -1;
            draw_leaderboard(leaderboardButtons, BTN_LEADERBOARD_COUNT, hoverIdx);
            
            if (handle_mouse_menu(leaderboardButtons, BTN_LEADERBOARD_COUNT, &hoverIdx)) {
                if (hoverIdx == 0) g_gameState = STATE_MENU;
                Sleep(200);
            }
            
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                 if (!escHeld) {
                     g_gameState = STATE_MENU;
                     escHeld = true;
                 }
            } else escHeld = false;
            
            Sleep(10);
        }
        else if (g_gameState == STATE_SETTINGS) {
            static int hoverIdx = -1;
            update_settings_buttons();
            draw_settings(settingsButtons, BTN_SETTINGS_COUNT, hoverIdx);
            
            // 使用专用的 handle_settings_mouse 以支持滑块
            if (handle_settings_mouse(settingsButtons, BTN_SETTINGS_COUNT, &hoverIdx)) {
                switch (hoverIdx) {
                    case 0: // 音乐开关
                        g_settings.musicEnabled = !g_settings.musicEnabled;
                        update_music();
                        save_settings(); // 保存设置
                        break;
                    case 1: // 重载音乐 (musics 文件夹)
                        scan_music_folder();
                        save_settings();
                        break;
                    case 2: // 重载背景 (images 文件夹)
                    {
                        // 重新扫描并加载
                        scan_images_folder();
                        save_settings(); // 保存设置
                    }
                    break;
            case 3: // 难度切换
                g_settings.difficulty = (Difficulty)((g_settings.difficulty + 1) % 3);
                save_settings(); // 保存设置
                break;
            case 4: // 不透明度滑块 (由 handle_settings_mouse 内部处理)
                break;
            case 5: // 音乐音量滑块 (由 handle_settings_mouse 内部处理)
                break;
            case 6: // 返回
                save_settings(); // 保存所有设置（包括滑块变动）
                g_gameState = STATE_MENU;
                break;
        }
        Sleep(200);
    }
            
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                 if (!escHeld) {
                     save_settings(); // 退出设置时保存
                     g_gameState = STATE_MENU;
                     escHeld = true;
                 }
            } else escHeld = false;
            
            Sleep(10);
        }
        else if (g_gameState == STATE_GAME_SINGLE || g_gameState == STATE_GAME_DUAL || g_gameState == STATE_GAME_LEVEL) {
            static bool justEntered = true;
            // 如果是 STATE_GAME_LEVEL，通常由选关界面进入，已经初始化了，所以这里只需要处理 SINGLE/DUAL 的初始化
            // 或者用一个标志位
            
            if (g_gameState != STATE_GAME_LEVEL && justEntered) {
                init_player(&g_player1, 1);
                if (selectedMode == MODE_DUAL) {
                    init_player(&g_player2, 2);
                }
                gameStartTime = clock();
                justEntered = false;
            }
            
            // 对于 STATE_GAME_LEVEL，justEntered 应该在进入状态前被重置，或者在状态切换时处理
            // 这里为了简单，假设选关界面进入时已经做好了初始化，我们只需要确保不重复初始化
            // 选关界面直接切换状态，所以这里不需要再 init
            // 但如果从 GameOver 重启 Level，需要处理
            
            // 检测暂停 (Space)
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                if (!spaceHeld) {
                    // 切换暂停状态
                    if (selectedMode == MODE_DUAL) {
                        g_player1.isPaused = !g_player1.isPaused;
                        g_player2.isPaused = !g_player2.isPaused;
                    } else {
                        g_player1.isPaused = !g_player1.isPaused;
                    }
                    spaceHeld = true;
                }
            } else spaceHeld = false;
            
            // 只有在未暂停时才处理输入和游戏逻辑
            bool isPaused = (selectedMode == MODE_DUAL) ? (g_player1.isPaused || g_player2.isPaused) : g_player1.isPaused;
            
            if (!isPaused) {
                handle_input(&g_player1, (selectedMode == MODE_DUAL) ? &g_player2 : NULL, selectedMode);
                update_game(&g_player1);
                if (selectedMode == MODE_DUAL) update_game(&g_player2);
                pauseHoverIdx = -1; // 重置悬停状态
            } else {
                // 暂停时处理鼠标交互
                if (handle_mouse_menu(pauseButtons, BTN_PAUSE_COUNT, &pauseHoverIdx)) {
                    switch (pauseHoverIdx) {
                        case 0: // 继续游戏
                            g_player1.isPaused = false;
                            if (selectedMode == MODE_DUAL) g_player2.isPaused = false;
                            break;
                        case 1: // 重新开始
                            if (g_gameState == STATE_GAME_LEVEL) {
                                // 关卡模式重启：重新初始化当前关卡
                                init_player(&g_player1, 1);
                                g_player1.currentLevelIndex = g_player1.currentLevelIndex; // 保持 ID
                                g_player1.levelTargetLines = g_levels[g_player1.currentLevelIndex].targetLines;
                                generate_level_garbage(&g_player1, (LevelID)g_player1.currentLevelIndex);
                            } else {
                                justEntered = true; // 普通模式重启
                            }
                            g_player1.isPaused = false;
                            if (selectedMode == MODE_DUAL) g_player2.isPaused = false;
                            break;
                        case 2: // 返回菜单
                            if (g_gameState == STATE_GAME_LEVEL) {
                                g_gameState = STATE_LEVEL_SELECT; // 关卡模式返回选关
                            } else {
                                g_gameState = STATE_MENU;
                            }
                            justEntered = true;
                            g_player1.isPaused = false;
                            if (selectedMode == MODE_DUAL) g_player2.isPaused = false;
                            break;
                    }
                    Sleep(200);
                }
            }
            
            // 检测退出 (ESC) -> 返回菜单
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                if (!escHeld) {
                    if (g_gameState == STATE_GAME_LEVEL) {
                        g_gameState = STATE_LEVEL_SELECT;
                    } else {
                        g_gameState = STATE_MENU;
                    }
                    justEntered = true; 
                    escHeld = true;
                    // 重置暂停
                    g_player1.isPaused = false; 
                    if (selectedMode == MODE_DUAL) g_player2.isPaused = false;
                }
            } else escHeld = false;
            
            // 检测胜负/结束
            if (selectedMode == MODE_SINGLE || selectedMode == MODE_LEVEL) {
                if (g_player1.isGameOver) {
                    if (selectedMode == MODE_SINGLE) add_to_leaderboard(g_player1.score);
                    g_gameState = STATE_GAMEOVER;
                }
            } else { // DUAL
                clock_t now = clock();
                int elapsedSec = (int)((now - gameStartTime) / CLOCKS_PER_SEC);
                
                // 暂停补偿
                if (g_player1.isPaused) {
                    gameStartTime += 16; 
                }

                // 如果 g_dualTimeLimit 为 -1，则timeLeft 设为 9999 (显示用)，并不触发超时
                int timeLeft = 0;
                if (g_dualTimeLimit != -1) {
                    timeLeft = g_dualTimeLimit - elapsedSec;
                } else {
                    timeLeft = 9999;
                }
                
                // 只有在有限时模式下才检查超时
                if (g_dualTimeLimit != -1 && timeLeft <= 0) {
                    if (g_player1.score > g_player2.score) { g_player1.isWinner = true; g_player2.isWinner = false; }
                    else if (g_player2.score > g_player1.score) { g_player2.isWinner = true; g_player1.isWinner = false; }
                    else { g_player1.isWinner = false; g_player2.isWinner = false; } // 平局
                    g_gameState = STATE_GAMEOVER;
                }
                else if (g_player1.isGameOver) {
                    g_player2.isWinner = true; g_player1.isWinner = false;
                    g_gameState = STATE_GAMEOVER;
                }
                else if (g_player2.isGameOver) {
                    g_player1.isWinner = true; g_player2.isWinner = false;
                    g_gameState = STATE_GAMEOVER;
                }
            }
            
            // 渲染
            clock_t now = clock();
            int elapsedSec = (int)((now - gameStartTime) / CLOCKS_PER_SEC);
            
            int timeLeft = 0;
            if (selectedMode == MODE_DUAL) {
                if (g_dualTimeLimit == -1) timeLeft = -1; // -1 表示无限
                else {
                    timeLeft = g_dualTimeLimit - elapsedSec;
                    if (timeLeft < 0) timeLeft = 0;
                }
            }
            
            if (g_gameState != STATE_GAMEOVER && g_gameState != STATE_GAME_LEVEL_WIN) {
                render_game(&g_player1, (selectedMode == MODE_DUAL) ? &g_player2 : NULL, selectedMode, timeLeft, elapsedSec, pauseButtons, BTN_PAUSE_COUNT, pauseHoverIdx);
                Sleep(16);
            } else {
                justEntered = true; 
            }
        }
        else if (g_gameState == STATE_GAMEOVER) {
            draw_game_over(&g_player1, (selectedMode == MODE_DUAL) ? &g_player2 : NULL, selectedMode);
            
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                if (!escHeld) {
                    if (selectedMode == MODE_LEVEL) {
                        g_gameState = STATE_LEVEL_SELECT;
                    } else {
                        g_gameState = STATE_MENU;
                    }
                    escHeld = true;
                }
            } else escHeld = false;
            
            if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                if (!enterHeld) {
                    // 重启相同模式
                    if (selectedMode == MODE_SINGLE) {
                        g_gameState = STATE_GAME_SINGLE;
                    } else if (selectedMode == MODE_LEVEL) {
                        // 重试当前关卡
                         start_level(&g_player1, 1, g_player1.currentLevelIndex);
                         g_gameState = STATE_GAME_LEVEL;
                    } else {
                        g_gameState = STATE_GAME_DUAL;
                    }
                    enterHeld = true;
                }
            } else enterHeld = false;
            
            Sleep(50);
        }
        else if (g_gameState == STATE_GAME_LEVEL_WIN) {
            // 关卡胜利状态
            render_game(&g_player1, NULL, MODE_LEVEL, 0, 0, NULL, 0, -1); // 继续渲染游戏画面（此时 render_game 会画 overlay）
            
            if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                if (!enterHeld) {
                    // 进入下一关
                    // currentLevelIndex 已经在 logic 中更新了吗？ logic 只负责解锁下一关
                    // 这里我们手动切换
                    int nextId = g_player1.currentLevelIndex + 1;
                    if (nextId < LEVEL_COUNT) {
                         init_player(&g_player1, 1);
                         g_player1.currentLevelIndex = nextId;
                         g_player1.levelTargetLines = g_levels[nextId].targetLines;
                         generate_level_garbage(&g_player1, (LevelID)nextId);
                         g_gameState = STATE_GAME_LEVEL;
                    } else {
                        // 全部通关
                        g_gameState = STATE_LEVEL_SELECT; // 或者显示通关画面
                    }
                    enterHeld = true;
                }
            } else enterHeld = false;
            
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                if (!escHeld) {
                    g_gameState = STATE_LEVEL_SELECT;
                    escHeld = true;
                }
            } else escHeld = false;
            
            Sleep(50);
        }
    }
    
    // 退出前保存所有设置
    save_settings();
    
    close_game_window();
    return 0;
}
