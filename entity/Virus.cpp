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
    // 保存当前绘图设备的变换矩阵、画笔颜色、字体、裁剪区域等所有状态。

    // 配合使用：它必须与 painter.restore() 成对出现。

    //实际效果：

    // save()：将当前设置“存档”在栈顶。

    // 之后你可以随意修改画笔颜色、旋转画布、缩放坐标。

    // restore()：一键将设置恢复到你 save() 那一刻的状态，避免后续绘图操作污染前面的设置。

    //注意：它不保存画布上已经画好的图形内容，只保存“作画的工具和视角”。
    painter.save();

    // 开启抗锯齿（Anti-aliasing）渲染效果，让绘制的图形（线条、圆形、文字边缘等）看起来更平滑、更柔和，减少“锯齿状”的毛边。

    // 没有抗锯齿（有锯齿）：就像用粗颗粒的乐高积木拼一条斜线。从远处看是斜的，凑近了看，边缘全是小方块堆出来的阶梯（一阶一阶的“狗牙”）。

    // 有抗锯齿（平滑）：就像用磨砂纸把乐高积木的直角边打磨成圆角，或者在阶梯上填充了过渡色。虽然还是那些方块，但视觉上边缘变得模糊、柔和，看起来就像一条真正的斜线了。
    painter.setRenderHint(QPainter::Antialiasing);

    // 3. 配置暗绿色边框与内嵌草绿色填充（大名鼎鼎的球球大作战经典刺球配色）
    QPen pen(QColor(0, 130, 20), 3 * scale); // 边框粗细也随相机缩放
    painter.setPen(pen);
    painter.setBrush(QColor(0, 210, 40));

    // 用多边形路径拼装“带刺的圆”
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
    thornPath.closeSubpath(); // 闭合路径


    painter.drawPath(thornPath);

    painter.restore();
}