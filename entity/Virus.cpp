#include "Virus.h"
#include <cmath>

Virus::Virus(float startX, float startY, float startMass, QColor c)
    : Ball(startX, startY,TYPE_VIRUS, startMass, c) {}

void Virus::draw(float cameraX, float cameraY, float screenWidth, float screenHeight, float scale, QPainter& painter) {
    // 1. 手写纯数学转换：把世界坐标转为屏幕视口坐标
    float screenX = (getX() - cameraX) * scale + screenWidth / 2.0f;
    float screenY = (getY() - cameraY) * scale + screenHeight / 2.0f;
    float screenR = getRadius() * scale; // 缩放后的标准半径

    // 🛡️ 视野裁剪：如果刺球完全不在屏幕内，直接不画，优化性能
    if (screenX + screenR < 0 || screenX - screenR > screenWidth ||
        screenY + screenR < 0 || screenY - screenR > screenHeight) {
        return;
    }

    painter.save();

    // 2. 开启抗锯齿，让尖刺看起来圆滑不刺眼
    painter.setRenderHint(QPainter::Antialiasing);

    // 3. 配置暗绿色边框与内嵌草绿色填充（大名鼎鼎的球球大作战经典刺球配色）
    QPen pen(QColor(0, 130, 20), 3 * scale); // 边框粗细也随相机缩放
    painter.setPen(pen);
    painter.setBrush(QColor(0, 210, 40));

    // 4. 🔥 核心算法：用多边形路径拼装“带刺的圆”
    QPainterPath thornPath;
    int spikeCount = 12; // 突刺的数量
    float spikeDepth = 12.0f * scale; // 尖刺的突起高度
    float twospikeDepth=2*spikeDepth;
    for (int i = 0; i < spikeCount * 2; ++i) {
        // 将 360 度均匀划分为 spikeCount * 2 个点
        float angle = i * (3.1415926f / spikeCount);

        // 奇数点向外伸出成尖，偶数点向内收缩成底
        float currentRadius = screenR;
        if (i % 2 == 0) {
            currentRadius -= spikeDepth;
        } else {
            currentRadius -= twospikeDepth;
        }

        // 三角函数求出每一个刺尖和刺底的屏幕绝对坐标
        float px = screenX + std::cos(angle) * currentRadius;
        float py = screenY + std::sin(angle) * currentRadius;

        if (i == 0) {
            thornPath.moveTo(px, py); // 起点
        } else {
            thornPath.lineTo(px, py); // 连线
        }
    }
    thornPath.closeSubpath(); // 闭合路径，形成完美齿轮

    // 5. 轰然落笔绘制
    painter.drawPath(thornPath);

    painter.restore();
}