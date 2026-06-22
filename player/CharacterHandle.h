#pragma once
#include <QPainter>
#include <QString>
#include "BasePlayer.h"
class BasePlayer;

class CharacterHandle {
public:
    // 🎯 渲染球心上的玩家基础名字和质量（零污染 Ball）
    static void drawBallText(QPainter& painter, BasePlayer* player);

    // 🎯 渲染屏幕固定位置的仪表盘：比如左上角的剩余生命、总得分/总质量
    static void drawScreenHUD(QPainter& painter, BasePlayer* player, float screenWidth, float screenHeight);

    // 🎯 渲染游戏结束（LifeCount == 0）时的最终大弹窗和结束语
    static void drawGameOverSummary(QPainter& painter, BasePlayer* player, float screenWidth, float screenHeight);
};