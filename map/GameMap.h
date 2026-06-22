#pragma once

class QPainter;

class GameMap {
private:
    static float mapWidth;
    static float mapHeight;

    static float gridWidth;
    static float gridHeight;

    static float screenWidth;
    static float screenHeight;
public:
    GameMap(float w = 2040, float h = 1240,float sw=1080,float sh =640);
    ~GameMap() = default;

    void init();
    void update(float deltaTime);
    void draw(float cameraX, float cameraY, float screenWidth, float screenHeight,float scale, QPainter& painter);


    static  float getMapWidth() ;
    static  float getMapHeight() ;
    static  float getscreenwidth() ;
    static  float getscreenheight() ;
};