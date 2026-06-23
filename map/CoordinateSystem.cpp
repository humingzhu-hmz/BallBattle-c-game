// map/CoordinateSystem.cpp
#include "CoordinateSystem.h"

void CoordinateSystem::worldToScreen(float worldX, float worldY,
                                     float camX, float camY,
                                     float screenW, float screenH,
                                     float scale,
                                     float& screenX, float& screenY)
{
    // 线性变换公式：
    // 1. (worldX - camX) 算出物体偏离相机焦点的绝对世界距离
    // 2. 乘以 scale 让其在视觉上向屏幕中心成比例地“收缩”或“延展”
    // 3. 加上固定不变的屏幕物理中心偏移量 (screenW / 2.0f)，使其完美平移到画布正中央
    screenX = (worldX - camX) * scale + screenW / 2.0f;
    screenY = (worldY - camY) * scale + screenH / 2.0f;
}

void CoordinateSystem::screenToWorld(float screenX, float screenY,
                                     float camX, float camY,
                                     float screenW, float screenH,
                                     float scale,
                                     float& worldX, float& worldY)
{
    // 💡 完美的逆向解方程：
    // 当鼠标在屏幕上点击时，我们需要把乘以 scale 的步骤逆向“除回来”，反推出世界真实坐标
    if (scale > 0.001f) { // 严谨保护：防止极端情况下的除零崩溃
        worldX = (screenX - screenW / 2.0f) / scale + camX;
        worldY = (screenY - screenH / 2.0f) / scale + camY;
    } else {
        worldX = camX;
        worldY = camY;
    }
}