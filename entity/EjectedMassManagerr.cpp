#include "EjectedMassManager.h"

EjectedMassManager::EjectedMassManager()
{
}

EjectedMassManager::~EjectedMassManager()
{
    for(EjectedMass* mass : masses)
        delete mass;

    masses.clear();
}

std::vector<EjectedMass*>& EjectedMassManager::getMasses()
{
    return masses;
}

void EjectedMassManager::add(EjectedMass* mass)
{
    masses.push_back(mass);
}

void EjectedMassManager::update(float deltaTime, float mapW, float mapH)
{
    for(EjectedMass* mass : masses)
    {
        if(mass)
            // 🎯 把真实的地图总宽高传进去，代替以前无意义的 0, 0
            mass->update(deltaTime, mapW, mapH);
    }
}

void EjectedMassManager::draw(float cameraX,float cameraY,float screenWidth,float screenHeight,float scale,QPainter& painter)
{
    for(EjectedMass* mass : masses)
    {
        if(mass)
            mass->draw(cameraX,cameraY,screenWidth,screenHeight,scale,painter);
    }
}