#pragma once
#include "../player/BasePlayer.h"
#include "../entity/FoodManager.h" // 引入食物管理器，让 AI 能看见豆豆

class AIPlayer : public BasePlayer {
public:
    // 构造函数同样直接透传
    AIPlayer(float startX, float startY, float startMass, float speed, QColor color);

    virtual ~AIPlayer() = default;

    // ✨【重写/重载决策层】：AI 真正的每帧思考入口，需要把环境（食物）传给它
    void updateAI(float deltaTime, FoodManager& foodManager,float mapWidth,float mapHeight);
};