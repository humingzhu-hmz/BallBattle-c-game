#include "Ball.h"
#include "../map/CoordinateSystem.h"
#include <cmath>

Ball::Ball(float startX, float startY, EntityType t, float startMass, QColor color)
    : x(startX), y(startY),type(t), mass(startMass), color(color)
{
    recalculateRadius(); // 这里会自动计算半径
}

// 外部碰撞中心调用该接口给球增重
void Ball::addMass(float amount) {
    if (amount > 0.0f) {
        mass += amount;
        recalculateRadius(); // 自动更新视觉半径
    }
}

// 核心守恒公式：半径 = 根号下质量
void Ball::recalculateRadius() {
    this->radius = std::sqrt(this->mass);
}

// Getter / Setter 实现
float Ball::getX() const { return x; }
float Ball::getY() const { return y; }
float Ball::getRadius() const { return radius; }
float Ball::getMass() const { return mass; }
void  Ball::setMass(float newMass) {
    this->mass=newMass;
    recalculateRadius();
}
QColor Ball::getColor() const { return color ;}
EntityType Ball::getType() const { return type;}
void Ball::setPosition(float newX, float newY) {
    x = newX;
    y = newY;
}
// player/Ball.cpp 内部的 draw 实现修改：

void Ball::draw(float cameraX, float cameraY, float screenWidth, float screenHeight, float scale, QPainter& painter)
{
    painter.setBrush(getColor());
    float scrX = 0.0f;
    float scrY = 0.0f;

    // 1. ✨【核心重构】：调用我们刚刚锁死的 CoordinateSystem，算出带缩放的屏幕像素中心点
    CoordinateSystem::worldToScreen(this->getX(), this->getY(),
                                    cameraX, cameraY,
                                    screenWidth, screenHeight,
                                    scale, // 将相机的全局屏幕/真实比例灌进来
                                    scrX, scrY);

    // 2. ✨【核心重构】：视觉半径作为空间的一部分，必须严格遵循相同的线性变换，乘以 scale
    float visualRadius = this->getRadius() * scale;

    // 3. 智能视口裁剪机制：用缩放后的“视觉半径”来精准判断球体当前是否在屏幕可见范围内
    if (scrX >= -visualRadius && scrX <= screenWidth + visualRadius &&
        scrY >= -visualRadius && scrY <= screenHeight + visualRadius) {

        // 4. 在屏幕上完美画出线性缩放后的圆
        painter.drawEllipse(QPointF(scrX, scrY), visualRadius, visualRadius);
    }
}