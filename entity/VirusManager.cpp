#include "VirusManager.h"
#include <random>

// 构造函数初始化上限
VirusManager::VirusManager(int maxCount)
    : maxVirusCount(maxCount) {}

VirusManager::~VirusManager() {
    for (Virus* v : viruses) {
        delete v;
    }
    viruses.clear();
}

std::vector<Virus*>& VirusManager::getViruses() {
    return viruses;
}

// 移除被炸毁或吃掉的刺球
void VirusManager::removeVirus(size_t index) {
    if (index < viruses.size()) {
        delete viruses[index];
        viruses.erase(viruses.begin() + index);
    }
}
void VirusManager::removeVirus(Ball* virusToRemove) {
    if (!virusToRemove) return;

    auto it=std::find(viruses.begin(),viruses.end(),virusToRemove);
    if(it!=viruses.end())
    {
        delete *it;
        viruses.erase(it);
    }
}
float VirusManager::getMinRadius()
{
    if(viruses.size()<1) return 0;
    float minr=viruses[0]->getRadius();
    for(int i=1;i<viruses.size();i++)
    {
        minr=std::min(minr,viruses[i]->getRadius());
    }
    return minr;
}
// 专门负责在大地图内部补充刺球
void VirusManager::spawnVirus(float mapWidth, float mapHeight) {
    if (static_cast<int>(viruses.size()) < maxVirusCount) {
        std::random_device rd;
        std::mt19937 gen(rd());

        // 留出 100 像素边缘，防止卡墙
        std::uniform_real_distribution<float> distX(100.0f, mapWidth - 100.0f);
        std::uniform_real_distribution<float> distY(100.0f, mapHeight - 100.0f);
        std::uniform_int_distribution<int>    distMass(2500,3000);
        while (static_cast<int>(viruses.size()) < maxVirusCount) {
            viruses.push_back(new Virus(distX(gen), distY(gen), distMass(gen),Qt::green));
        }
    }
}

// 生命周期更新：每帧检查并补足刺球数量
void VirusManager::update(float mapWidth, float mapHeight) {
    spawnVirus(mapWidth, mapHeight);
}
// 视野窗口内渲染刺球
void VirusManager::draw(float cameraX, float cameraY, float screenWidth, float screenHeight, float scale, QPainter& painter) {
    //painter.setPen(Qt::darkBlue); // 删掉或留空这行，我们在子类内部自己掌控了画笔
    for (Virus* v : viruses) {
        v->draw(cameraX, cameraY, screenWidth, screenHeight, scale, painter);
    }
}