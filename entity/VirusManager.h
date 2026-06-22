#pragma once
#include "Virus.h"
#include <vector>
#include <QPainter>

class VirusManager {
private:
    std::vector<Virus*> viruses;
    int maxVirusCount; // 👈 新增：大地图维持的刺球上限
    float  MinRadius;
public:
    VirusManager(int maxCount); // 👈 修改：构造函数传入最大数量
    ~VirusManager();

    // 核心业务函数
    void spawnVirus(float mapWidth, float mapHeight); // 👈 新增：负责生成刺球
    void removeVirus(size_t index);                    // 👈 新增：移除刺球
    void removeVirus(Ball* v);
    // 提供给 CollisionManager 调用的接口
    std::vector<Virus*>& getViruses();
    float getMinRadius();
    // 生命周期与绘制
    void update(float mapWidth, float mapHeight);
    void draw(float cameraX, float cameraY, float screenWidth, float screenHeight,float scale, QPainter& painter);
};