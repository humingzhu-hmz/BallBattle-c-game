#include "AIManager.h"
#include <cstdlib>
#include "../map/GameMap.h"
AIManager::AIManager(int maxaicount):maxAICount(maxaicount)
{
    spawnAI(GameMap::getMapWidth(),GameMap::getMapHeight());
}

AIManager::~AIManager() {
    clear();
}

void AIManager::spawnAI(float mapWidth, float mapHeight) {
    QColor colors[] = { Qt::green, Qt::blue, Qt::magenta, Qt::cyan, Qt::yellow };

    // ✨ 核心修正：不再一上来就清空容器，而是有多少就保留多少，只补齐差额！
    while (static_cast<int>(aiPlayers.size()) < maxAICount) {
        float rx = static_cast<float>(rand() % static_cast<int>(mapWidth));
        float ry = static_cast<float>(rand() % static_cast<int>(mapHeight));
        QColor rColor = colors[rand() % 5];

        aiPlayers.push_back(new AIPlayer(rx, ry, 400.0f, 400.0f, rColor));
    }
}

void AIManager::updateAllAI(float deltaTime, FoodManager& foodManager, float mapWidth, float mapHeight) {

    // 正常的 AI 运动与决策更新以及ai人员更换,不是通过之前删除管理的ai的对象来达到目的了,而是通过重新覆盖这个ai对象,这是因为之前我们
    // 没有真正能够删除Aiplayer对象,这是因为这个remove逻辑是由basePlayer来实现的
    //所以Aimanager也就不需要这个spawnAI了,我们统一将spawn逻辑交给基类basePlayer,这样只需要判断当前这个aiplayer或者player是否存活就可以了
    for (AIPlayer* ai : aiPlayers) {
        if (ai) ai->updateAI(deltaTime, foodManager,mapWidth,mapHeight);
    }
}

void AIManager::draw(float cameraX, float cameraY, float screenWidth, float screenHeight, float scale, QPainter& painter) {
    for (AIPlayer* ai : aiPlayers) {
        if (ai) {
            ai->draw(cameraX, cameraY, screenWidth, screenHeight, scale, painter);
        }
    }
}

std::vector<AIPlayer*>& AIManager::getAIPlayers() {
    return aiPlayers;
}

void AIManager::clear() {
    for (AIPlayer* ai : aiPlayers) {
        delete ai;
    }
    aiPlayers.clear();
}