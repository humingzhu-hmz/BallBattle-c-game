#include "OperateManager.h"
#include "../../player/BasePlayer.h"
#include "../../Music/MusicManager.h"
#include <cmath>
#include <algorithm> // 确保引入 std::find
#include "../../entity/EjectedMass.h"
#include <qDebug>
void OperateManager::split(BasePlayer* player, float targetX, float targetY)
{
    if (!player) return;
    auto& balls = player->getBalls();

    // 1. 已经是上限 16 个球，直接拒绝
    if (balls.size() >= 16)
        return;

    std::vector<Ball*> newBalls;

    // 2. 拷贝一份旧的球指针列表进行遍历，防止逻辑中潜在的容器干扰
    std::vector<Ball*> oldBalls = balls;

    for (Ball* ball : oldBalls)
    {
        if (!ball) continue;

        // 如果当前总球数已经达到或超过 16，立刻彻底终止
        if (balls.size() + newBalls.size() >= 16)
            break;

        float mass = ball->getMass();
        if (mass < 800.0f) // 分裂阈值
            continue;

        float x = ball->getX();
        float y = ball->getY();

        float dx = targetX - x;
        float dy = targetY - y;
        float len = std::sqrt(dx * dx + dy * dy);

        if (len < 0.001f) continue;

        dx /= len;
        dy /= len;

        // 本体质量减半
        float newMass = mass * 0.5f;
        ball->setMass(newMass); // 内部会自动触发 recalculateRadius()

        // 计算新球生成的初始距离（母球新半径 * 1.2 偏移，防止重叠产生严重物理抖动）
        float spawnDistance = ball->getRadius() * 1.2f;

        float newX = x + dx * spawnDistance;
        float newY = y + dy * spawnDistance;

        // 创建可控的子球
        ControllableBall* newBall = new ControllableBall(
            newX,
            newY,
            newMass,
            400.0f, // 初始速度或推力系数
            player->getColor() // ✨ 适配父类公开接口获取颜色
            );

        // 给新球施加一个向外的爆炸性推力（4000.0f 获得破空而出的弹射手感）
        newBall->applyExplosionPush(
            dx * 4000.0f,
            dy * 4000.0f
            );

        // 🎯【注入官方合体 CD】：根据分裂后的实际质量动态计算
        float cooldownTime = 0.5 + (newMass * 0.000001f);

        // 设定上限 2 分钟，防止百万质量需要等好几个小时
        if (cooldownTime > 2) {
            cooldownTime = 2.0f;
        }

        // 让这一对母球 and 新子球同步进入合体冷却 CD
        ball->setMergeCooldown(cooldownTime);
        newBall->setMergeCooldown(cooldownTime);

        // 将新球放入临时容器
        newBalls.push_back(newBall);
    }

    // 3. 将新球批量塞回当前玩家/AI 的控制列表中
    if (!newBalls.empty()) {
        balls.insert(balls.end(), newBalls.begin(), newBalls.end());
    }
    //Music::getInstance().playSplit();
}

void OperateManager::eject(BasePlayer* player, float targetX, float targetY, std::vector<EjectedMass*>& masses)
{
    if (!player) return;

    auto& balls = player->getBalls();
    constexpr float ejectSpeed = 900.0f; // 孢子飞行的初速度

    for (Ball* ball : balls)
    {
        if (!ball) continue;

        float currentMass = ball->getMass();

        // 1. 动态计算本次吐球的质量
        float dynamicEjectMass = currentMass * 0.1f;

        // 给吐出来的质量设一个合理的区间
        if (dynamicEjectMass < 32.0f)   dynamicEjectMass = 32.0f;
        if (dynamicEjectMass > 3000.0f) dynamicEjectMass = 3000.0f;

        // 核心限制：如果母球扣掉这个质量后，会低于基础存活质量 800 + 孢子质量，则拒绝喷吐
        if (currentMass <= 800.0f + dynamicEjectMass) continue;

        float x = ball->getX();
        float y = ball->getY();

        float dx = targetX - x;
        float dy = targetY - y;
        float len = std::sqrt(dx * dx + dy * dy);

        if (len < 0.001f) continue;

        dx /= len;
        dy /= len;

        // 2. 扣除当前母球质量
        ball->setMass(currentMass - dynamicEjectMass);

        // 3. 计算生成位置：母球新半径 + 孢子自身半径 + 安全距离
        float sporeRadius = 4.0f * std::sqrt(dynamicEjectMass);
        float spawnX = x + dx * (ball->getRadius() + sporeRadius + 10.0f);
        float spawnY = y + dy * (ball->getRadius() + sporeRadius + 10.0f);

        // 4. 🎯 直接用熟悉的 push_back 塞进 vector
        masses.push_back(
            new EjectedMass(
                spawnX,
                spawnY,
                dynamicEjectMass,
                ball->getColor(),
                dx * ejectSpeed,
                dy * ejectSpeed
                )
            );
    }
    //Music::getInstance().playEject();
}

// 🎯 ✨【这里是全新实现的静态合并逻辑】
void OperateManager::checkAndMerge(BasePlayer* player, float deltaTime)
{
    if (!player) return;
    auto& playerBalls = player->getBalls();
    if (playerBalls.size() <= 1) return; // 单球不需要合体

    // 1. 统一递减该角色所有子球的合体 CD 倒计时
    for (Ball* ball : playerBalls) {
        if (ball) ball->updateCooldown(deltaTime);
    }

    // 2. 双重循环检查该角色内部的任意两颗子球
    for (size_t i = 0; i < playerBalls.size(); ++i)
    {
        for (size_t j = i + 1; j < playerBalls.size(); ++j)
        {
            Ball* ballA = playerBalls[i];
            Ball* ballB = playerBalls[j];

            if (!ballA || !ballB) continue;

            // 铁律：有任意一颗球还在 CD 状态（比如那 2 秒内），坚决不给融合
            if (!ballA->canMerge() || !ballB->canMerge()) {
                continue;
            }

            // 计算这两颗子球的中心距
            float dx = ballB->getX() - ballA->getX();
            float dy = ballB->getY() - ballA->getY();
            float distance = std::sqrt(dx * dx + dy * dy);
            float combinedRadius = ballA->getRadius() + ballB->getRadius();

            // 🎯 ✨【融合条件】：过了 CD，且两颗球只要发生了重叠（哪怕只挨到一点边缘）
            if (distance < combinedRadius)
            {
                // 决定谁是主球（质量大的是捕食者，质量小的是食物）
                Ball* larger = (ballA->getMass() >= ballB->getMass()) ? ballA : ballB;
                Ball* smaller = (larger == ballA) ? ballB : ballA;

                // 🎯 ✨【核心动画：每一帧进行质量转移】
                // 每一帧，大球从小球身上抽走一部分质量
                // 比如每秒抽走小球当前剩余质量的 300%（即乘上系数 3.0f），让缩小的视觉更明显
                float transferMass = smaller->getMass() * 3.0f * deltaTime;

                // 设定每帧抽取的保底值，防止小球无限趋近于 0 却吸不干净
                if (transferMass < 15.0f) {
                    transferMass = 15.0f;
                }

                // 如果小球剩下的质量已经不够这次抽了，就全抽走
                if (transferMass >= smaller->getMass()) {
                    transferMass = smaller->getMass();
                }

                // 3. 真正执行质量扣除与增加
                // 内部会自动触发你的重新计算半径逻辑，因此在视觉上你一定会看到：
                // 大球开始不断吹气变大，小球开始迅速坍塌、急速减小！
                larger->setMass(larger->getMass() + transferMass);
                smaller->setMass(smaller->getMass() - transferMass);

                // 🎯 ✨【死亡判定】：当小球质量被完全抽干（低于极小值）时，安全销毁它
                if (smaller->getMass() <= 10.0f)
                {
                    auto it = std::find(playerBalls.begin(), playerBalls.end(), smaller);
                    if (it != playerBalls.end()) {
                        playerBalls.erase(it);
                    }
                    delete smaller; // 彻底释放小球内存
                    return; // 容器结构已变，立刻结束本帧遍历，防止迭代器崩溃
                }
            }
        }
    }
    //Music::getInstance().playMerge();
}