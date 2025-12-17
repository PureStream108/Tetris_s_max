#ifndef LEVELS_H
#define LEVELS_H

#include "common.h"

// 关卡 ID 定义
typedef enum {
    LEVEL_1 = 0,
    LEVEL_2,
    LEVEL_3,
    LEVEL_4,
    LEVEL_5,
    LEVEL_6,
    LEVEL_7,
    LEVEL_8,
    LEVEL_9,
    LEVEL_10,
    LEVEL_EX_1,
    LEVEL_EX_2,
    LEVEL_EX_3,
    LEVEL_COUNT
} LevelID;

// 初始垃圾行类型
typedef enum {
    GARBAGE_NONE,
    GARBAGE_RANDOM,  // 随机空洞
    GARBAGE_CONVEX,  // 凸型 (山)
    GARBAGE_CONCAVE  // 凹型 (谷)
} GarbageType;

// 关卡数据结构
typedef struct {
    LevelID id;
    TCHAR name[16];           // 如 "PM-1"
    TCHAR title[32];          // 如 "PM-1 初出茅庐"
    TCHAR story[1024];        // 剧情文本
    
    int targetLines;         // 目标消除行数
    int initialGarbageRows;  // 初始垃圾行数
    GarbageType garbageType; // 垃圾行类型
    bool restrictPieces;     // 是否仅生成 Z/S (针对 EX-3)
    
    bool isUnlocked;         // 是否已解锁
    bool isCompleted;        // 是否已通关
    
    // UI 位置 (在选关地图上的坐标)
    int mapX;
    int mapY;
} LevelData;

// 全局关卡数据数组
extern LevelData g_levels[LEVEL_COUNT];

// 函数声明
void init_levels(); // 初始化关卡数据
void unlock_next_level(LevelID currentId);
void generate_level_garbage(PlayerState *p, LevelID id); // 生成关卡初始垃圾
LevelData* get_level_data(LevelID id);
bool is_level_completed(LevelID id);
void set_level_completed(LevelID id, bool completed);

#endif // LEVELS_H
