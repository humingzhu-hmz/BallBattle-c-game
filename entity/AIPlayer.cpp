#include "AIPlayer.h"
#include <cmath>
#include <limits>

AIPlayer::AIPlayer(float startX, float startY, float startMass, float speed, QColor color)
    : BasePlayer(startX, startY, startMass, speed, color) {}

void AIPlayer::updateAI(float deltaTime, FoodManager& foodManager,float mapWidth,float mapHeight) {
    if(getlifeStatue()==false)
    {
        spawnPlayer(mapWidth,mapHeight);
    }
    // 1. 获取 AI 阵营当前的质心坐标（父亲已经帮你算好了）
    float aiX = getCenterX();
    float aiY = getCenterY();
    float aiR = getMinRadius();
    // 2. 开启“雷达扫描”：遍历全图食物，寻找最近的一颗
    std::vector<Food*>& foods = foodManager.getFoods();

    float closestDist = std::numeric_limits<float>::max();
    float targetX = aiX; // 默认目标为自己原地，防止没食物时乱飞
    float targetY = aiY;

    for (Food* food : foods) {
        if (!food) continue;

        //不去到达不了的地方
        float foodx=food->getX();
        float foody=food->getY();


        float dx = foodx - aiX;
        float dy = foody - aiY;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        // 简略判断,以弧线段的弦来判断
        if(dist>aiR)
        {
            // 假设左下角是(0,0)
            // 判断右上角45度线,x+y>w+h-r
            // 右下角45度线,x-y>w-r
            // 左下角,x+y<r
            // 左上角,y-x>h-r
            if(((foodx+foody)<aiR)||((foodx+foody)>mapWidth+mapHeight-aiR))
                continue;
            else if(((foodx-foody)>mapWidth-aiR)||((foody-foodx)>mapHeight-aiR))
                continue;
        }
        // 如果找到了更近的食物，更新锁定目标
        if (dist < closestDist) {
            closestDist = dist;
            targetX = foodx;
            targetY = foody;
        }
    }

    // 3. 🎯【灵魂交接】：把雷达锁定好的最优目标点，结结实实地喂给父亲的通用物理引擎
    // 这样就会直接触发你写在 BasePlayer 里的子球移动、越胖越慢、防抖死区等全套物理规律！
    update(deltaTime, targetX, targetY);
}