#pragma once
#include "../player/Ball.h"
#include <QColor>

// 完美继承 Ball，具备 x, y 和 radius 属性
class Food : public Ball {
private:
public:
    // 构造函数声明
    Food(float startX, float startY, float startmass, QColor c);

    // 析构函数声明（若基类 Ball 的析构是虚函数，这里建议显式声明）
    virtual ~Food() = default;

};