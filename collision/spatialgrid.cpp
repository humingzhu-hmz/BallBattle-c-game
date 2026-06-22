#include "SpatialGrid.h"
#include <cmath>
#include <algorithm>

SpatialGrid::SpatialGrid(float mapWidth, float mapHeight, float cellSize)
    : cellSize(cellSize) {
    // 根据地图大小计算需要多少列和多少行
    cols = static_cast<int>(std::ceil(mapWidth / cellSize));
    rows = static_cast<int>(std::ceil(mapHeight / cellSize));
    cells.resize(cols * rows);
}

void SpatialGrid::clear() {
    for (auto& cell : cells) {
        cell.clear();
    }
}

void SpatialGrid::insert(Ball* ball, BasePlayer* owner) {
    if (!ball) return;

    // 计算球的 AABB 包围盒覆盖了哪几个格子
    int startCol = std::max(0, static_cast<int>((ball->getX() - ball->getRadius()) / cellSize));
    int endCol = std::min(cols - 1, static_cast<int>((ball->getX() + ball->getRadius()) / cellSize));
    int startRow = std::max(0, static_cast<int>((ball->getY() - ball->getRadius()) / cellSize));
    int endRow = std::min(rows - 1, static_cast<int>((ball->getY() + ball->getRadius()) / cellSize));

    // 将球塞进所有它碰到的格子里
    for (int r = startRow; r <= endRow; ++r) {
        for (int c = startCol; c <= endCol; ++c) {
            cells[r * cols + c].push_back({ball, owner});
        }
    }
}

std::vector<GridNode> SpatialGrid::getNearby(Ball* ball) {
    std::vector<GridNode> nearby;
    if (!ball) return nearby;

    int startCol = std::max(0, static_cast<int>((ball->getX() - ball->getRadius()) / cellSize));
    int endCol = std::min(cols - 1, static_cast<int>((ball->getX() + ball->getRadius()) / cellSize));
    int startRow = std::max(0, static_cast<int>((ball->getY() - ball->getRadius()) / cellSize));
    int endRow = std::min(rows - 1, static_cast<int>((ball->getY() + ball->getRadius()) / cellSize));

    for (int r = startRow; r <= endRow; ++r) {
        for (int c = startCol; c <= endCol; ++c) {
            for (const auto& node : cells[r * cols + c]) {
                // 排除自己
                if (node.ball != ball) {
                    nearby.push_back(node);
                }
            }
        }
    }

    // 去重操作（因为一个大球可能同时跨越多个格子，会被多次添加）
    std::sort(nearby.begin(), nearby.end(), [](const GridNode& a, const GridNode& b) {
        return a.ball < b.ball;
    });
    auto last = std::unique(nearby.begin(), nearby.end(), [](const GridNode& a, const GridNode& b) {
        return a.ball == b.ball;
    });
    nearby.erase(last, nearby.end());

    return nearby;
}