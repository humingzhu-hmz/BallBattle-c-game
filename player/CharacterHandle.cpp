#include "CharacterHandle.h"

// 1. 画在球心里的名字（干净、清爽）
// 1. 画在球心里的名字（纯名字，无任何数字污染）
void CharacterHandle::drawBallText(QPainter& painter, BasePlayer* player) {
    if (!player || !player->getlifeStatue()) return;
    auto& balls = player->getBalls();
    if (balls.empty()) return;

    Ball* mainBall = balls[0];
    if (!mainBall) return;

    painter.save();

    // 🎯 ✨【核心修改：完全解除字号限制，纯粹自由跟随质量/半径】
    // 这里的 0.3f 是字号系数。如果觉得球变大时字长得还不够快，可以调大到 0.35f 或 0.4f
    // 这样质量（半径）越大，字体就会无上限地跟着变大，绝对不固定！
    int fontSize = std::max(15,static_cast<int>(mainBall->getRadius() * 0.5f));

    // 保底给 1 个像素，防止半径极小时出现 0 号字引发崩溃
    if (fontSize < 1) fontSize = 1;

    QFont font("Microsoft YaHei", fontSize, QFont::Bold);
    painter.setFont(font);
    painter.setPen(Qt::white); // 纯白中文名

    float r = mainBall->getRadius();
    QRectF textRect(mainBall->getX() - r, mainBall->getY() - r, r * 2, r * 2);

    // 只画干净的中文名字
    QString text = player->getName();

    painter.drawText(textRect, Qt::AlignCenter, text);
    painter.restore();
}

// 2. 画在屏幕角落的常驻看板（生命次数、当前总质量）
void CharacterHandle::drawScreenHUD(QPainter& painter, BasePlayer* player, float screenWidth, float screenHeight) {
    if (!player) return;

    painter.save();

    // 在左上角（15, 15）挂一个精致的半透明黑色底卡
    painter.setBrush(QColor(0, 0, 0, 100)); // 半透明黑
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(15, 15, 200, 80, 10, 10); // 圆角矩形看板

    // 写看板文字
    QFont font("Microsoft YaHei", 11, QFont::Bold);
    painter.setFont(font);
    painter.setPen(Qt::white);

    // 展现总质量和剩余生命数
    QString infoStr = QString(" 玩家: %1\n 💰 总质量: %2\n ❤️ 剩余生命: %3")
                          .arg(player->getName())
                          .arg(static_cast<int>(player->getSumMass()))
                          .arg(player->getlifeCount());

    painter.drawText(QRect(20, 20, 190, 70), Qt::AlignLeft | Qt::AlignVCenter, infoStr);
    painter.restore();
}

// 3. 画在屏幕正中间的“游戏结束”霸屏提示与结束语
void CharacterHandle::drawGameOverSummary(QPainter& painter, BasePlayer* player, float screenWidth, float screenHeight) {
    if (!player || player->getlifeCount() > 0) return; // 还有命就不触发

    painter.save();

    // 1. 铺上一层全局的暗红/深黑半透明滤镜，渲染死亡氛围
    painter.fillRect(0, 0, screenWidth, screenHeight, QColor(20, 0, 0, 180));

    // 2. 绘制中央结算大卡片
    float cardW = 450;
    float cardH = 250;
    float cardX = (screenWidth - cardW) / 2.0f;
    float cardY = (screenHeight - cardH) / 2.0f;

    painter.setBrush(QColor(30, 30, 30, 240)); // 极深灰卡片背景
    painter.setPen(QPen(Qt::red, 3)); // 血红边框
    painter.drawRoundedRect(cardX, cardY, cardW, cardH, 15, 15);

    // 3. 喷涂大标题
    QFont fontTitle("Microsoft YaHei", 24, QFont::Bold);
    painter.setFont(fontTitle);
    painter.setPen(Qt::red);
    painter.drawText(QRectF(cardX, cardY + 20, cardW, 50), Qt::AlignCenter, "GAME OVER");

    // 4. 喷涂精美的最终游戏结束语结算信息
    QFont fontBody("Microsoft YaHei", 13, QFont::Normal);
    painter.setFont(fontBody);
    painter.setPen(Qt::white);

    QString summaryStr = QString("很遗憾，您的生命次数已耗尽！\n\n"
                                 "本局最终玩家: %1\n")
                             .arg(player->getName());

    painter.drawText(QRectF(cardX + 20, cardY + 80, cardW - 40, 140), Qt::AlignCenter, summaryStr);

    painter.restore();
}