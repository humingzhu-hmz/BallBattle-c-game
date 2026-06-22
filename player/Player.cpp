#include "Player.h"
#include <QDebug>
#include "../map/Camera.h"
Player::Player(float startX, float startY, float startMass, float speed,int lifeCount, QColor color)
    : BasePlayer(startX, startY, startMass, speed, color),lifeCount(lifeCount) {};
int Player::getlifeCount()
{
    return lifeCount;
}
void Player::split(float targetX,float targetY)
{
    auto& balls = getBalls();

    constexpr int MAX_BALLS = 16;

    if(balls.size() >= MAX_BALLS)
        return;

    std::vector<Ball*> newBalls;

    for(Ball* ball : balls)
    {
        if(!ball)
            continue;

        float mass = ball->getMass();

        // 太小不允许分裂
        if(mass < 80.0f)
            continue;

        float newMass = mass * 0.5f;

        ball->setMass(newMass);

        float x = ball->getX();
        float y = ball->getY();

        float dx = targetX - x;
        float dy = targetY - y;

        float len = std::sqrt(dx * dx + dy * dy);

        if(len < 0.001f)
            continue;

        dx /= len;
        dy /= len;

        auto* child =
            new ControllableBall(x + dx * 50.0f,y + dy * 50.0f,newMass,400.0f,getColor());

        child->applyExplosionPush(dx * 1200.0f,dy * 1200.0f);

        newBalls.push_back(child);
    }

    for(Ball* ball : newBalls)
    {
        balls.push_back(ball);
    }
    qDebug()
        << "ball count ="
        << balls.size();
}
void Player::eject(float targetX,float targetY)
{
    auto& balls = getBalls();

    for(Ball* ball : balls)
    {
        if(!ball)
            continue;

        if(ball->getMass() < 100.0f)
            continue;

        ball->addMass(-16.0f);
    }
}
void Player::merge()
{

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