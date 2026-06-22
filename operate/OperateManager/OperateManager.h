#pragma once

#include <vector>

class BasePlayer;
class Player;
class EjectedMass;

class OperateManager
{
public:
    // 分裂逻辑
    static void split(BasePlayer* player, float targetX, float targetY);

    // 🎯 ✨【方案二】：参数回归 vector，保持极简和直观
    static void eject(BasePlayer* player, float targetX, float targetY, std::vector<EjectedMass*>& masses);

    // 官方合体冷却与融合逻辑
    static void checkAndMerge(BasePlayer* player, float deltaTime);
};