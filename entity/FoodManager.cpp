#include "FoodManager.h"
#include <random>
#include <cmath>

// 构造函数：初始化最大食物数量
FoodManager::FoodManager(int maxCount)
    : maxFoodCount(maxCount) {}

// 析构函数：释放 vector 中动态分配的 Food 内存，防止内存泄漏
FoodManager::~FoodManager() {
    for (Food* f : foods) {
        delete f;
    }
    foods.clear();
}

// 获取食物列表的引用
std::vector<Food*>& FoodManager::getFoods() {
    return foods;
}

// 被吃掉时，安全释放内存并从容器中移除
void FoodManager::removeFood(size_t index) {
    if (index < foods.size()) {
        delete foods[index];
        foods.erase(foods.begin() + index);
    }
}
void FoodManager::removeFood(Ball* foodToRemove) {
    if (!foodToRemove) return;

    // std::find 依然可以使用，因为你在重载或比较时，对比的是指针地址
    auto& foodList = getFoods();
    auto it = std::find(foodList.begin(), foodList.end(), foodToRemove);

    if (it != foodList.end()) {
        delete *it;
        foodList.erase(it);
    }
}
// 对应设计图规划：专门负责在大地图内部补充食物
void FoodManager::spawnFood(float mapWidth, float mapHeight) {
    if (static_cast<int>(foods.size()) < maxFoodCount) {
        std::random_device rd;
        std::mt19937 gen(rd());
        // rd() 初始化种子,gen是一个随机数生成器
        // std::uniform_real_distribution<float> distX(a,b),相当于指定distX(gen),这个产生的随机数范围在a-b

        // 留出 15 像素边缘，防止食物正好刷在最外圈粗黑墙体上
        std::uniform_real_distribution<float> distX(15.0f, mapWidth - 15.0f);
        std::uniform_real_distribution<float> distY(15.0f, mapHeight - 15.0f);
        std::uniform_int_distribution<int>    distColor(50, 255);
        std::uniform_int_distribution<int>    distMass(50,100);

        while (static_cast<int>(foods.size()) < maxFoodCount) {
            QColor randomColor(distColor(gen), distColor(gen), distColor(gen));
            // 传入坐标、初始质量、颜色
            foods.push_back(new Food(distX(gen), distY(gen), distMass(gen), randomColor));
        }
    }
}

// 生命周期更新
void FoodManager::update(float mapWidth, float mapHeight) {
    spawnFood(mapWidth, mapHeight);
}

// 视野窗口内渲染食物
void FoodManager::draw(float cameraX, float cameraY, float screenWidth, float screenHeight,float scale, QPainter& painter) {

    painter.setPen(Qt::NoPen);
    for (Food* f : foods) {
        f->draw(cameraX, cameraY, screenWidth , screenHeight, scale,painter);
    }
}