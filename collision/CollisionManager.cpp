#include "CollisionManager.h"
#include <cmath>
#include <QDebug>
// 1. 纯几何圆形碰撞判定
bool CollisionManager::checkBallCollision(const Ball& ballA, const Ball& ballB) {
    float dx = ballA.getX() - ballB.getX();
    float dy = ballA.getY() - ballB.getY();
    float distance = std::sqrt(dx * dx + dy * dy);
    return distance < (ballA.getRadius() + ballB.getRadius() * 0.3f);
}

// 2. ✨【实现】：全阵营通用边界卡位逻辑
void CollisionManager::checkMapBoundaries(BasePlayer& anyPlayer, float mapWidth, float mapHeight) {
    for (Ball* ball : anyPlayer.getBalls()) {
        if (!ball) continue;
        float bx = ball->getX();
        float by = ball->getY();
        float r = ball->getRadius();

        if (bx < r) bx = r;
        if (bx > mapWidth - r) bx = mapWidth - r;
        if (by < r) by = r;
        if (by > mapHeight - r) by = mapHeight - r;

        ball->setPosition(bx, by);
    }
}

// 3. 通用阵营吃食物业务
void CollisionManager::checkPlayerVsFood(BasePlayer& anyPlayer, FoodManager& foodManager, SpatialGrid& grid) {
    std::vector<Ball*>& playerBalls = anyPlayer.getBalls();

    // 遍历玩家的所有分身
    for (int i = 0; i < (int)playerBalls.size(); ++i) {
        Ball* playerBall = playerBalls[i];
        if (!playerBall) continue;

        // 🚀 核心：向空间网格索要当前球周围的实体
        std::vector<GridNode> nearbyNodes = grid.getNearby(playerBall);

        for (const GridNode& node : nearbyNodes) {
            // ⚠️ 统一规范：使用类型枚举判断，彻底抛弃 dynamic_cast
            // 只要不是食物，直接跳过，性能极高
            if (node.ball->getType() != TYPE_FOOD) continue;

            // 这里可以直接用 node.ball，因为它是 Ball 基类指针，已经具备 getX/getY/getMass 等方法
            Ball* food = node.ball;

            // 几何距离判断 (使用距离平方，避免 sqrt 开方运算，提升性能)
            float dx = playerBall->getX() - food->getX();
            float dy = playerBall->getY() - food->getY();
            float distanceSq = dx * dx + dy * dy;
            float combinedRadius = playerBall->getRadius() + food->getRadius();

            // 🎯 碰撞判定
            if (distanceSq < combinedRadius * combinedRadius) {

                // 1. 吃掉食物增加质量
                playerBall->addMass(food->getMass());

                // 2. 移除食物
                foodManager.removeFood(food);

                // 3. 性能优化：吃掉一个后立刻停止对该 playerBall 的遍历
                break;
            }
        }
    }
}

// 4. 通用阵营与刺球碰撞业务
void CollisionManager::checkPlayerVsVirus(BasePlayer& anyPlayer, VirusManager& virusManager, SpatialGrid& grid) {
    std::vector<Ball*>& playerBalls = anyPlayer.getBalls();

    // 遍历玩家的所有分身
    for (int i = 0; i < (int)playerBalls.size(); ++i) {
        Ball* ball = playerBalls[i];
        if (!ball) continue;

        // 🚀 向网格要数据
        std::vector<GridNode> nearbyNodes = grid.getNearby(ball);

        for (const GridNode& node : nearbyNodes) {
            // 类型识别（假设你的 Virus 有一个判断方法，或者你可以用 dynamic_cast）
            if (node.ball->getType() != TYPE_VIRUS) continue;
            Ball* virus =node.ball;
            float dx = ball->getX() - virus->getX();
            float dy = ball->getY() - virus->getY();
            float distance = std::sqrt(dx * dx + dy * dy);

            // 🎯 只要半径够大且中心重叠
            if (ball->getRadius() > virus->getRadius() && distance < ball->getRadius()) {

                // 【关键修改】：这里 i 就是当前的 ballIndex
                anyPlayer.explodeBall(i, virus);

                // 移除刺球
                virusManager.removeVirus(virus);

                // 一个子球一次只能吃一个刺，避免连续吃导致的问题，break 掉
                break;
            }
        }
    }
}

// 5. 通用阵营与通用阵营（人与人、人与AI、AI与AI）吞噬业务
void CollisionManager::checkAllPlayerCollisions(std::vector<BasePlayer*>& allPlayers, SpatialGrid& grid) {
    float AteFactor = 1.08f;

    for (BasePlayer* playerA : allPlayers) {
        if (!playerA || !playerA->getlifeStatue()) continue;

        std::vector<Ball*>& ballsA = playerA->getBalls();

        // 逆序遍历，防止吞噬移除时索引越界,这里当player被吃完了,此时也绝对不会越界,因为此时一定以最后一个
        // ballsA.size()决定了,最多可以被吃多少次,被吃完了,这个for (int i = static_cast<int>(ballsA.size()) - 1; i >= 0; --i)
        // 循环刚好结束,不用担心访问到playerA为空的时候,这就是逆序遍历的妙处啊
        for (int i = static_cast<int>(ballsA.size()) - 1; i >= 0; --i) {
            Ball* ballA = ballsA[i];
            if (!ballA) continue;

            // 🚀 核心：直接从网格里把周围可能碰到的球“捞”出来，不用比对全场！
            std::vector<GridNode> nearbyNodes = grid.getNearby(ballA);

            for (const GridNode& node : nearbyNodes) {
                Ball* ballB = node.ball;
                BasePlayer* playerB = node.owner;

                if (!ballB || ballA == ballB||!playerB) continue; // 不能撞自己
                // 找了许久的bug就在这里啊,playerB没有做排除处理啊,spatialGrid里面food,virus都是nullptr
                // 所以这里开始没有做!playerB导致食物被吃的逻辑也发生在了这里啊,进而又导致playerB->remove(balla)直接炸
                // nullptr->() [X].大错啊
                float rA = ballA->getRadius();
                float rB = ballB->getRadius();

                // 🟩 AABB 正方形极速过滤（不用开平方根）
                if (std::abs(ballA->getX() - ballB->getX()) > (rA + rB) ||
                    std::abs(ballA->getY() - ballB->getY()) > (rA + rB)) {
                    continue; // 正方形没相交，绝对碰不上
                }

                // 🟡 只有正方形相交了，才进行圆心距精确计算
                float dx = ballA->getX() - ballB->getX();
                float dy = ballA->getY() - ballB->getY();
                float distance = std::sqrt(dx * dx + dy * dy);

                bool isSamePlayer = (playerA == playerB);

                // 🎯【分支 1】：处理同体分身刚性排斥
                if (isSamePlayer) {
                    float overlap = (rA + rB) * 0.95f - distance;
                    if (overlap > 0.0f && distance > 0.1f) {
                        float pushX = (dx / distance) * overlap * 0.5f;
                        float pushY = (dy / distance) * overlap * 0.5f;
                        ballA->setPosition(ballA->getX() + pushX, ballA->getY() + pushY);
                        ballB->setPosition(ballB->getX() - pushX, ballB->getY() - pushY);
                    }
                    continue;
                }

                // 🎯【分支 2】：真正的异体残酷吞噬
                float massA = ballA->getMass();
                float massB = ballB->getMass();

                if (massA > massB * AteFactor && distance < ballA->getRadius()) {
                    ballA->addMass(massB);
                    // qDebug()
                    //     << "playerA=" << playerA
                    //     << "playerB=" << playerB
                    //     << "ballA=" << ballA
                    //     << "ballB=" << ballB;
                    // qDebug()
                    //     << playerB->getBalls().size();
                    playerB->removeBall(ballB);
                } else if (massB > massA * AteFactor && distance < ballB->getRadius()) {
                    ballB->addMass(massA);
                    // qDebug()
                    //     << "playerA=" << playerA
                    //     << "playerB=" << playerB
                    //     << "ballA=" << ballA
                    //     << "ballB=" << ballB;
                    // qDebug()
                    //     << playerB->getBalls().size();
                    playerA->removeBall(ballA); // 注意：由于是从后往前遍历 ballA，当前 ballA 被吃也安全
                }
            }
        }
    }
}

void CollisionManager::checkSelfPushback(BasePlayer& anyPlayer) {
    std::vector<Ball*>& playerBalls = anyPlayer.getBalls();
    int ballCount = playerBalls.size();

    // 如果只有一个球，不需要进行内部排斥判断
    if (ballCount <= 1) return;

    // ⚙️ 磨合期物理刚性参数
    const float REPOSITION_PERCENT = 0.8f;   // 位置修正系数 (80%消除重叠，防止抖动)
    const float BOUNCE_FACTOR = 0.2f;        // 重叠时的反弹冲力强度

    // 🎯 两两组合遍历 (O(N^2) 精确计算子球间的挤压)
    for (int i = 0; i < ballCount; ++i) {
        Ball* ballA = playerBalls[i];
        if (!ballA) continue;

        for (int j = i + 1; j < ballCount; ++j) {
            Ball* ballB = playerBalls[j];
            if (!ballB) continue;

            // 1. 计算两个子球的中心点距离
            float dx = ballB->getX() - ballA->getX();
            float dy = ballB->getY() - ballA->getY();
            float distance = std::sqrt(dx * dx + dy * dy);
            float combinedRadius = ballA->getRadius() + ballB->getRadius();

            // 2. 判定是否发生重叠
            if (distance < combinedRadius) {
                // 安全防御：防止完全重合时除以 0
                if (distance == 0.0f) {
                    dx = 1.0f;
                    dy = 0.0f;
                    distance = 1.0f;
                }

                // 计算重叠深度
                float overlap = combinedRadius - distance;

                // 计算推开的方向法线 (单位向量)
                float nx = dx / distance;
                float ny = dy / distance;

                // 3. 核心物理阶段一：基于质量比的强行位置修正（消除重叠）
                float massA = ballA->getMass();
                float massB = ballB->getMass();
                float totalMass = massA + massB;

                // 大球沉稳推不动，小球灵动被弹开
                float ratioA = massB / totalMass;
                float ratioB = massA / totalMass;

                float correctionAmount = overlap * REPOSITION_PERCENT;

                // 直接修正坐标，强制让它们贴靠在一起，不准重叠
                ballA->setPosition(ballA->getX() - nx * correctionAmount * ratioA,
                                   ballA->getY() - ny * correctionAmount * ratioA);
                ballB->setPosition(ballB->getX() + nx * correctionAmount * ratioB,
                                   ballB->getY() + ny * correctionAmount * ratioB);

                // 4. 核心物理阶段二：通过你写的 applyExplosionPush 接口注入动态磨合阻力
                // 转化为 ControllableBall 指针以应用冲力
                ControllableBall* cBallA = static_cast<ControllableBall*>(ballA);
                ControllableBall* cBallB = static_cast<ControllableBall*>(ballB);

                // 给它们施加相互排斥的弹射冲力，对抗鼠标的聚拢吸力
                float pushForceA = correctionAmount * ratioA * BOUNCE_FACTOR;
                float pushForceB = correctionAmount * ratioB * BOUNCE_FACTOR;

                cBallA->applyExplosionPush(-nx * pushForceA, -ny * pushForceA);
                cBallB->applyExplosionPush(nx * pushForceB, ny * pushForceB);
            }
        }
    }
}