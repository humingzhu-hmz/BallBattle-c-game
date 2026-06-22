#pragma once

#include "../player/Ball.h"

class EjectedMass : public Ball
{
private:
    float vx;
    float vy;
public:
    EjectedMass(float x,float y,float mass,QColor color,float vx,float vy);

    virtual void update(float deltaTime,float targetX,float targetY) override;
};