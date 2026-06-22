// map/CoordinateSystem.h
#pragma once

class CoordinateSystem {
public:
    // 【核心重构】：转换到屏幕坐标时，融入缩放因子 scale
    static void worldToScreen(float worldX, float worldY,
                              float camX, float camY,
                              float screenW, float screenH,
                              float scale, // 👈 新增：相机当前的平滑缩放比例
                              float& screenX, float& screenY);

    // 【核心重构】：从屏幕坐标反推世界坐标时，同样需要剔除缩放因子 scale
    static void screenToWorld(float screenX, float screenY,
                              float camX, float camY,
                              float screenW, float screenH,
                              float scale, // 👈 新增：反推鼠标世界坐标时也需要它
                              float& worldX, float& worldY);
};