#pragma once
#include "SpatialGrid.h"
#include "../entity/FoodManager.h"
#include "../entity/VirusManager.h"
class CollisionManager {
public:
    // 1. 【底层基石】：纯几何圆形碰撞计算
    static bool checkBallCollision(const Ball& ballA, const Ball& ballB);

    // 2. ✨【新引入】：全阵营通用地图物理边界刚性卡位（把AI和玩家的边界逻辑统一收拢到这！）
    static void checkMapBoundaries(BasePlayer& anyPlayer, float mapWidth, float mapHeight);

    // 3. 🎯【核心重载一】：处理 任意阵营(BasePlayer) 与 食物 的碰撞业务
    static void checkPlayerVsFood(BasePlayer& anyPlayer, FoodManager& foodManager, SpatialGrid& grid);

    // 4. 🎯【核心重载二】：处理 任意阵营(BasePlayer) 与 刺球(Virus) 的碰撞业务
    static void checkPlayerVsVirus(BasePlayer& anyPlayer, VirusManager& virusManager, SpatialGrid& grid);

    // 5. 🎯【核心重载三】：处理 任意阵营 与 任意阵营 之间的残酷吞噬业务（人吃AI、AI吃人、AI吃AI全包）
    static void checkAllPlayerCollisions(std::vector<BasePlayer*>& allPlayers, SpatialGrid& grid);
    // 6.  处理子球在炸开之后应该排斥一段时间之后的问题,不应该马上合并
    static void checkSelfPushback(BasePlayer& anyPlayer);
};