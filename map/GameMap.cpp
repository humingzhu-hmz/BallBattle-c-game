#include "GameMap.h"
#include <QPainter>
// map/GameMap.cpp
#include "CoordinateSystem.h" // 👈 记得引入坐标系统

float GameMap::mapWidth = 4080.0f;
float GameMap::mapHeight = 2480.0f;

float GameMap::gridWidth = 40.80f;
float GameMap::gridHeight = 24.80f;

float GameMap::screenWidth = 1080.0f;
float GameMap::screenHeight = 640.0f;
GameMap::GameMap(float w, float h, float sw, float sh)
{
    mapWidth = w;
    mapHeight = h;

    screenWidth = sw;
    screenHeight = sh;

    gridWidth = mapWidth/100;
    gridHeight = mapHeight/100;
}

void GameMap::init() {}
void GameMap::update(float deltaTime) {}

// map/GameMap.cpp 内部优化版绘制逻辑

void GameMap::draw(float cameraX, float cameraY, float screenWidth, float screenHeight, float scale, QPainter& painter) {
    float topLeftX = 0.0f, topLeftY = 0.0f;
    float bottomRightX = 0.0f, bottomRightY = 0.0f;

    // 1. 依然先用坐标系统算出大地图的左上角和右下角的像素边界（这四个点作为大坝边界雷打不动）
    CoordinateSystem::worldToScreen(0.0f, 0.0f, cameraX, cameraY, screenWidth, screenHeight, scale, topLeftX, topLeftY);
    CoordinateSystem::worldToScreen(mapWidth, mapHeight, cameraX, cameraY, screenWidth, screenHeight, scale, bottomRightX, bottomRightY);

    // 2. 填充白底大地图并设置裁剪
    QRectF mapRect(topLeftX, topLeftY, bottomRightX - topLeftX, bottomRightY - topLeftY);
    painter.fillRect(mapRect, Qt::white);

    painter.save();
    painter.setClipRect(mapRect);

    // 统一线宽与颜色
    painter.setPen(QPen(QColor(220, 220, 220), qMax(1.5f, 1.0f * scale)));

    // ========================================================
    // ⚡【极致优化：垂直网格线】纯加法等差迭代
    // ========================================================
    // 算出第一条垂直线（worldX = 0）在屏幕上的初始像素 X 坐标，其实就是 topLeftX
    float currentScrX = topLeftX;
    // 算出缩放后的屏幕像素等差步长
    float xStep = gridWidth * scale;

    for (float worldX = 0; worldX <= mapWidth; worldX += gridWidth) {
        // 直接享用等差加法带来的极致平滑，免去一切函数调用开销
        painter.drawLine(QPointF(currentScrX, topLeftY), QPointF(currentScrX, bottomRightY));
        currentScrX += xStep; // 纯加法驱动
    }

    // ========================================================
    // ⚡【极致优化：水平网格线】纯加法等差迭代
    // ========================================================
    // 算出第一条水平线（worldY = 0）在屏幕上的初始像素 Y 坐标，其实就是 topLeftY
    float currentScrY = topLeftY;
    // 算出缩放后的屏幕像素等差步长
    float yStep = gridHeight * scale;

    for (float worldY = 0; worldY <= mapHeight; worldY += gridHeight) {
        // 同理，免去 dummyX 等无用变量的创建和转换
        painter.drawLine(QPointF(topLeftX, currentScrY), QPointF(bottomRightX, currentScrY));
        currentScrY += yStep; // 纯加法驱动
    }

    painter.restore();

    // 3. 外墙粗黑框
    float strokeWidth = qMax(2.0f, 8.0f * scale);
    painter.setPen(QPen(Qt::black, strokeWidth, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter.drawRect(mapRect);
}


float GameMap::getMapWidth() { return mapWidth; }
float GameMap::getMapHeight() { return mapHeight; }
float GameMap::getscreenwidth() { return screenWidth; }
float GameMap::getscreenheight() { return screenHeight; }