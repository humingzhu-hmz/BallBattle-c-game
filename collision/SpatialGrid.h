#pragma once
#include <vector>
#include "../player/BasePlayer.h"

// 绑定球和它的主人，方便后续判断是不是同一个人
struct GridNode {
    Ball* ball;
    BasePlayer* owner;
};

class SpatialGrid {
private:
    float cellSize;
    int cols;
    int rows;
    std::vector<std::vector<GridNode>> cells;

public:
    // 构造函数：传入地图的宽、高，以及单个格子的尺寸（推荐 200 到 300）
    SpatialGrid(float mapWidth, float mapHeight, float cellSize);

    // 每帧开头清空网格
    void clear();

    // 将球按其包围盒（AABB）插入到对应的格子里
    void insert(Ball* ball, BasePlayer* owner);

    // 获取这颗球所在格子及其周围格子里所有的潜在碰撞目标
    std::vector<GridNode> getNearby(Ball* ball);
};