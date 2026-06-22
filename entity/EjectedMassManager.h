#pragma once

#include <vector>
#include <QPainter>

#include "EjectedMass.h"

class EjectedMassManager
{
private:
    std::vector<EjectedMass*> masses;

public:
    EjectedMassManager();
    ~EjectedMassManager();

    std::vector<EjectedMass*>& getMasses();

    void add(EjectedMass* mass);

    // 让 update 能够接收地图的大小
    void update(float deltaTime, float mapW, float mapH);

    void draw(float cameraX,float cameraY,float screenWidth,float screenHeight,float scale,QPainter& painter);
};