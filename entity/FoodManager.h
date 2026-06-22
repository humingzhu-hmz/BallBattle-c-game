#pragma once
#include "Food.h"
#include <vector>
#include <QPainter>
class FoodManager {
private:
    std::vector<Food*> foods;
    int maxFoodCount;

public:
    // 构造与析构函数声明
    FoodManager(int maxCount);
    ~FoodManager();

    // 获取食物列表的引用
    std::vector<Food*>& getFoods();

    // 核心业务函数声明
    void spawnFood(float mapWidth, float mapHeight);// 补充食物
    void removeFood(size_t index);
    void removeFood(Ball* f);
    void update(float mapWidth, float mapHeight);
    void draw(float cameraX, float cameraY, float screenWidth, float screenHeight,float scale, QPainter& painter);
};