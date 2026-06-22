// map/Camera.cpp
#include "Camera.h"
#include <algorithm>
#include <cmath>
float Camera::x=1240;
float Camera::y=640;
float Camera::targetX=1240;
float Camera::targetY=640;
Camera::Camera(float startX, float startY,float scale)
    : lerpSpeed(5.0f),currentScale(scale), targetScale(scale), lastTriggerMass(400.0f)
{
    x=startX;
    y=startY;
    targetX=startX;
    targetY=startY;
}

void Camera::setPosition(float newX, float newY) {
    x = newX;  y= newY; targetX = newX; targetY = newY;
}

void Camera::setTarget(float tX, float tY) {
    targetX = tX; targetY = tY;
}

void Camera::update(float deltaTime, float mapWidth, float mapHeight, float screenWidth, float screenHeight, float currentTotalMass) {
    // 1. 相机坐标平滑逼近目标点
    x += (targetX - x) * lerpSpeed * deltaTime;
    y += (targetY - y) * lerpSpeed * deltaTime;

    // 2. 动态计算当前视口的半宽和半高
    float halfViewportW = screenWidth / 2.0f;
    float halfViewportH = screenHeight / 2.0f;

    // 3. 动态计算 5% 的自适应黑边缓冲距离
    float padding = screenWidth * 0.05f;

    // 4. 锁死相机边界
    x = std::max(halfViewportW - padding, std::min(x, mapWidth - halfViewportW + padding));
    y = std::max(halfViewportH - padding, std::min(y, mapHeight - halfViewportH + padding));

    // ========================================================
    // ✨【高阶重构：相机自主控制的非线性缩放逻辑】
    // ========================================================
    if (currentTotalMass < 400.0f) currentTotalMass = 400.0f; // 安全保底

    // 临界无感区控制：玩家质量变动率超过当前底数的 5% 时，才触发目标视野变更
    if (std::abs(currentTotalMass - lastTriggerMass) / lastTriggerMass > 0.05f) {

        // 采用完美契合大球演变的【对数收拢公式】
        // 400时为1.0，增重越快缩放拉远越敏感，中后期趋于平缓
        targetScale = 1.0f / (1.0f + 0.35f * std::log(currentTotalMass / 400.0f));

        // 视野极限锁死，防止无限拉远导致游戏变像素画
        if (targetScale < 0.35f) targetScale = 0.35f;
        if (targetScale > 1.0f)  targetScale = 1.0f;

        lastTriggerMass = currentTotalMass; // 更新基准
    }

    // 5. 阻尼过渡：当前画面比例通过 Lerp 动画慢慢“滑向”目标比例
    // 这里的 3.0f 代表视野拉伸过渡的柔和度，数值越大镜头缩放跟随越快
    currentScale += (targetScale - currentScale) * 3.0f * deltaTime;
}

float Camera::getX() const { return x; }
float Camera::getY() const { return y; }
float Camera::getScale() const { return currentScale; } // 吐给全图的唯一合法比例