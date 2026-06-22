#pragma once
#include "../player/Ball.h"
#include <QPainter>
#include <QPainterPath>
class Virus : public Ball {
public:
    Virus(float startX, float startY, float startMass, QColor c);
    virtual ~Virus() = default;

    // 🔥 重写基类的 draw 函数，实现自定义尖刺渲染
    void draw(float cameraX, float cameraY, float screenWidth, float screenHeight, float scale, QPainter& painter);
};