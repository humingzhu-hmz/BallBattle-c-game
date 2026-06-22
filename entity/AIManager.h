#pragma once
#include <vector>
#include "AIPlayer.h"
#include <QPainter>

class AIManager {
private:
    std::vector<AIPlayer*> aiPlayers; // 把 AI 军团的所有权收归 AIManager
    int maxAICount;                   // 由 AIManager 自己掌控最大数量限制
public:
    AIManager(int maxAICount=15);
    ~AIManager();

    // 1. 初始化刷出 AI
    void spawnAI(float mapWidth, float mapHeight);

    // 2. 核心更新：内部去迭代每一个 AI 跑雷达或状态机
    void updateAllAI(float deltaTime, FoodManager& foodManager, float mapWidth, float mapHeight);

    // 4. 统一渲染
    void draw(float cameraX, float cameraY, float screenWidth, float screenHeight, float scale, QPainter& painter);

    // 5. 提供外部获取容器的接口，用于碰撞管理器判定
    std::vector<AIPlayer*>& getAIPlayers();

    // 6. 清理释放
    void clear();
};