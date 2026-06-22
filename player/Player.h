#pragma once
#include "BasePlayer.h"
class Player : public BasePlayer {
    int lifeCount=1;
public:
    Player(float startX, float startY, float startMass, float speed,int lifecount, QColor color,QString name);
    // 更新玩家状态,调用basePlayer.update更新旗下所有球,同时根据现在是否死亡来判断是否重新刷新我们的球
    virtual void updatePlayer(float deltaTime, float targetX, float targetY,float mapWidth,float mapHeight) ;
    int getlifeCount() override;
    virtual ~Player() = default;
};