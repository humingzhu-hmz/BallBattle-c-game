#include "EjectedMass.h"
#include <cmath>

// 1. 构造函数：利用初始化列表将坐标、类型、质量、颜色透传给父类 Ball
EjectedMass::EjectedMass(float x, float y, float mass, QColor color, float vx, float vy)
    : Ball(x, y, TYPE_EJECTED_MASS, mass, color), vx(vx), vy(vy)
{
}

// 2. 物理运动与边界锁死逻辑
void EjectedMass::update(float deltaTime, float targetX, float targetY)
{
    // ✨ 这里的 targetX 和 targetY 我们可以灵活借用来传递地图的 mapw 和 maph！
    // 这样既不需要修改基类的虚函数签名，又能拿到边界值。
    float mapW = targetX;
    float mapH = targetY;

    // A. 基础速度衰减（让吐出来的球带有一点滑行刹车感，逐渐慢下来）
    // 每秒速度会自然衰减，防止小球无限飞下去
    vx *= std::pow(0.92f, deltaTime * 60.0f);
    vy *= std::pow(0.92f, deltaTime * 60.0f);

    // B. 计算下一帧的预测世界坐标
    float nextX = this->getX() + vx * deltaTime;
    float nextY = this->getY() + vy * deltaTime;
    float r = this->getRadius();

    // C. 🎯【核心修复】：边缘碰撞拦截并锁死，同时将速度置零
    if (nextX < r) {
        nextX = r;
        vx = 0.0f;
    }
    if (nextX > mapW - r) {
        nextX = mapW - r;
        vx = 0.0f;
    }
    if (nextY < r) {
        nextY = r;
        vy = 0.0f;
    }
    if (nextY > mapH - r) {
        nextY = mapH - r;
        vy = 0.0f;
    }

    // D. 灌回最新坐标
    this->setPosition(nextX, nextY);
}