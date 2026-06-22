#include "Player.h"
#include <QDebug>
#include "../map/Camera.h"
Player::Player(float startX, float startY, float startMass, float speed,int lifeCount, QColor color)
    : BasePlayer(startX, startY, startMass, speed, color),lifeCount(lifeCount) {};
int Player::getlifeCount()
{
    return lifeCount;
}
void Player::updatePlayer(float deltaTime, float targetX, float targetY,float mapWidth,float mapHeight)
{
    if(getlifeStatue()==false&&lifeCount>0)
    {
        qDebug()
        << getlifeStatue()
        << lifeCount;
        lifeCount--;
        spawnPlayer(mapWidth,mapHeight);
        // 先刷新再更新camera 中心点
        Camera::setPosition(getCenterX(),getCenterY());
    }
    else if(getlifeStatue()&&lifeCount==0)
    {
        // 游戏结束台词
    }
    update(deltaTime,targetX,targetY);
}