#include "BasePlayer.h"
#include <qcolor.h>
#include <random>
#include <algorithm> // 记得在 cpp 顶部包含
#include <QDebug>
#include "../Music/MusicManager.h"
#include "../operate/OperateManager//OperateManager.h"
// ========================================================
// ControllableBall（会动的球）实现
// ========================================================
ControllableBall::ControllableBall(float startX, float startY, float startMass,float speed,QColor color)
    : Ball(startX, startY,TYPE_PLAYER_BALL, startMass,color),speed(speed){}// 透传给基类构造

void ControllableBall::update(float deltaTime, float targetX, float targetY) {
    float currentX = getX();
    float currentY = getY();
    float dx = targetX - currentX;
    float dy = targetY - currentY;
    float distance = std::sqrt(dx * dx + dy * dy);

    float currentActiveSpeed = 0.0f;
    // 1. 初始化这一帧由鼠标或雷达带来的“常规控制位移”
    float controlMovementX = 0.0f;
    float controlMovementY = 0.0f;

    if (distance > 1.0f) {
        // 核心物理参数
        float initialSpeed = 600.0f;
        float speedFloor = 400.0f;
        float speedDecayExponent = 0.5f;

        // 高阶幂次衰减算法与速度同步
        float normalizedMass = std::max(1.0f, getMass() / 400.0f);
        float speedRatio = std::exp(-std::pow(normalizedMass, speedDecayExponent));
        this->setSpeed(speedFloor + (initialSpeed - speedFloor) * speedRatio);

        // 读取速度与死区缓动控制
        this->getSpeed(currentActiveSpeed);

        if (distance < 2.0f) {
            currentActiveSpeed = 1.0f; // 你原本的极近距离保底速度
        } else if (distance < 30.0f) {
            currentActiveSpeed *= (distance - 2.0f) / 28.0f; // 线性减速带
        }

        // 计算出常规控制位移的分量
        controlMovementX = (dx / distance) * currentActiveSpeed * deltaTime;
        controlMovementY = (dy / distance) * currentActiveSpeed * deltaTime;
    }

    // ========================================================
    // 🔥 【核心融入点】：计算刺球爆炸带来的弹射惯性物理（独立于控制逻辑）
    // ========================================================

    // 2. 算上爆炸带来的冲力位移
    float explosionMovementX = pushVx * deltaTime;
    float explosionMovementY = pushVy * deltaTime;

    // 3. 惯性高频衰减（模拟地面摩擦力，让滑行逐渐停下来）
    // pushDecay 可以在构造函数里初始化为 0.85f 左右
    pushVx *= std::pow(pushDecay, deltaTime * 60.0f);
    pushVy *= std::pow(pushDecay, deltaTime * 60.0f);

    // 阻尼保底，防止无限趋近于 0 导致微小的浮点数内耗
    if (std::abs(pushVx) < 1.0f) pushVx = 0.0f;
    if (std::abs(pushVy) < 1.0f) pushVy = 0.0f;

    // ========================================================
    // 4. 最终位置平移：将 [常规控制] 和 [爆炸冲击] 完美融合
    // ========================================================
    float nextX = currentX + controlMovementX + explosionMovementX;
    float nextY = currentY + controlMovementY + explosionMovementY;
    // 塞在你 update 函数的最后：
    // if (getMass() > 100.0f) { // 只观察大球
    //     qDebug() << "Mass:" << getMass()
    //              << " Dist:" << distance
    //              << " Speed:" << currentActiveSpeed
    //              << " MoveX:" << controlMovementX
    //              << " MoveY:" << controlMovementY;
    // }
    setPosition(nextX, nextY);
}
void ControllableBall::getSpeed(float& acceptSpeed) const
{
    acceptSpeed=this->speed;
}
void ControllableBall::setSpeed(float newSpeed)
{
    this->speed=newSpeed;
}
void ControllableBall::applyExplosionPush(float vx, float vy) {
    pushVx = vx;
    pushVy = vy;
}

// ========================================================
// BasePlayer（玩家灵魂）实现
// ========================================================
int BasePlayer::counts=0;
void BasePlayer::applyMassDecay(float deltaTime)
{
    counts++;
    counts %= 20;

    // 每20帧执行一次
    if (counts)
        return;

    constexpr float MIN_MASS = 400.0f;

    for (Ball* ball : myBalls)
    {
        if (!ball)
            continue;

        float mass = ball->getMass();

        if (mass <= MIN_MASS)
            continue;

        float lossRate;

        if (mass < 1000.0f)
        {
            lossRate = 0.001f;
        }
        else if (mass < 3000.0f)
        {
            lossRate = 0.003f;
        }
        else if (mass < 5000.0f)
        {
            lossRate = 0.008f;
        }
        else if (mass < 10000.0f)
        {
            lossRate = 0.02f;
        }
        else
        {
            lossRate = 0.05f;
        }

        float loss =
            mass *
            lossRate *
            deltaTime *
            20.0f;

        float newMass = mass - loss;

        if (newMass < MIN_MASS)
            newMass = MIN_MASS;

        ball->setMass(newMass);
    }
}
BasePlayer::BasePlayer(float startX, float startY,float startMass,float speed,QColor color)
    : centerX(startX), centerY(startY),color(color),initMass(startMass),initSpeed(speed),lifeStatue(true)
{
    // 利用多态指针：new 出来运动子类，塞进基类容器
    minRadius=sqrt(startMass);
    myBalls.push_back(new ControllableBall(startX, startY,startMass,speed,color));
}

BasePlayer::~BasePlayer() {
    for (Ball* ball : myBalls) {
        delete ball;
    }
    myBalls.clear();
}
void BasePlayer::spawnPlayer(float mapWidth,float mapHeight)
{
    // 1. 清空上一条命残留的球（防止内存泄漏）
    for (Ball* b : myBalls) {
        if (b) delete b;
    }
    myBalls.clear();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(15.0f, mapWidth - 15.0f);
    std::uniform_real_distribution<float> distY(15.0f, mapHeight - 15.0f);
    std::uniform_int_distribution<int>    distColor(0, 255);
    QColor randomColor(distColor(gen), distColor(gen), distColor(gen));
    centerX=distX(gen);
    centerY=distY(gen);
    color=randomColor;
    lifeStatue=true;

    myBalls.push_back(new ControllableBall(centerX,centerY,initMass,initSpeed,color));
    Music::getInstance().playRespawn();
}
void BasePlayer::update(float deltaTime, float mouseWorldX, float mouseWorldY) {
    if (myBalls.empty()) return;
    float weightedSumX = 0.0f;
    float weightedSumY = 0.0f;
    float totalMass = 0.0f;
    for (Ball* ball : myBalls) {
        // 核心虚函数多态调用：程序会自动执行 ControllableBall::update,更新单个球向哪里移动
        ball->update(deltaTime, mouseWorldX, mouseWorldY);

        // 根据各子球的真实质量做物理质心权重累加
        float m = ball->getMass();
        weightedSumX += ball->getX() * m;
        weightedSumY += ball->getY() * m;
        totalMass += m;
    }
    if (totalMass > 0.0f) {
        centerX = weightedSumX / totalMass;
        centerY = weightedSumY / totalMass;
    }
    applyMassDecay(deltaTime);
}

float BasePlayer::getCenterX()
{
    return centerX;
}
float BasePlayer::getCenterY()
{
    return centerY;
}
bool BasePlayer::getlifeStatue ()
{
    return lifeStatue;
}
float BasePlayer::getMinRadius()
{
    for(Ball* b:myBalls)
    {
        minRadius=std::min(minRadius,b->getRadius());
    }
    return minRadius;
}
void BasePlayer::removeBall(int index) {
    // 🛡️ 安全防御：防止外部传入非法索引导致程序崩溃
    if (index < 0 || index >= static_cast<int>(myBalls.size())) {
        return;
    }

    // 1. 释放该索引指向的 Ball 对象的物理内存
    if (myBalls[index] != nullptr) {
        delete myBalls[index];
        myBalls[index] = nullptr; // 良好的习惯：释放后立即置空
    }

    // 2. 将指针从 vector 容器中彻底移除（这会自动让后面的元素向前平移）
    myBalls.erase(myBalls.begin() + index);

    // 3. 🛡️ 状态清算：如果球被扣光了，直接改变生命状态
    if (myBalls.empty()) {
        lifeStatue = false; // 阵亡
    }
}

void BasePlayer::removeBall(Ball* ball) {
    if (ball == nullptr) return; // 🛡️ 空指针直接返回

    // 1. 在容器中查找这个指针的位置
    auto it = std::find(myBalls.begin(), myBalls.end(), ball);

    // 2. 如果找到了对应的球
    if (it != myBalls.end()) {
        // 先释放这颗球的物理内存
        delete *it;

        // 再将这个失效的指针从 vector 中擦除
        myBalls.erase(it);
    }
    else return;

    // 3. 🛡️ 状态清算
    if (myBalls.empty()) {
        lifeStatue = false;
    }
}
std::vector<Ball*>& BasePlayer::getBalls() {
    return myBalls;
}

void BasePlayer::draw(float cameraX, float cameraY, float screenWidth, float screenHeight,float scale, QPainter& painter)
{
    painter.setPen(Qt::NoPen);
    for(Ball* ball:myBalls)
    {
        ball->draw(cameraX, cameraY, screenWidth, screenHeight,scale, painter);
    }
}
// ========================================================
// BasePlayer 质量特征获取接口实现
// ========================================================

// 1. 获取玩家所有分裂子球的总质量总和
float BasePlayer::getSumMass() {
    float totalMass = 0.0f;
    for (Ball* ball : myBalls) {
        if (ball) { // 严谨保护：防止空指针
            totalMass += ball->getMass();
        }
    }
    return totalMass;
}

// 2. ✨【独立相机视野驱动核心】：获取当前体积/质量最大的那一颗球的质量
float BasePlayer::getMaxBallMass() {
    if (myBalls.empty()) return 400.0f; // 如果没球，返回初始保底质量

    float maxMass = 0.0f;
    for (Ball* ball : myBalls) {
        if (ball && ball->getMass() > maxMass) {
            maxMass = ball->getMass();
        }
    }
    return maxMass;
}
QString BasePlayer::getName() const{ return name;}
void BasePlayer::setName(QString newName) { name=newName; }
void BasePlayer::updateMassDecay(float deltaTime)
{
    auto& playerBalls = this->getBalls();

    constexpr float BASE_MASS = 400.0f;       // 初始低保质量
    constexpr float BASE_DECAY_RATE = 0.002f;  // 基础流失率

    for (Ball* ball : playerBalls)
    {
        if (!ball) continue;

        float currentMass = ball->getMass();
        if (currentMass <= BASE_MASS) continue; // 低保质量不衰减

        // 🎯 幂函数动态流失率
        float scaleFactor = std::pow(currentMass / BASE_MASS, 0.4f);
        float dynamicRate = BASE_DECAY_RATE * scaleFactor;

        // 限制单秒流失率上限（每秒最多流失当前质量的 8%）
        if (dynamicRate > 0.08f) {
            dynamicRate = 0.08f;
        }

        // 计算当前帧流失的绝对质量
        float massToLose = currentMass * dynamicRate * deltaTime;

        // 执行扣除并兜底
        float newMass = currentMass - massToLose;
        if (newMass < BASE_MASS) {
            newMass = BASE_MASS;
        }

        ball->setMass(newMass);
    }
}
QColor BasePlayer::getColor() const { return color;}
void BasePlayer::explodeBall(int ballIndex, Ball* virus) {
    // 🛡️ 安全检查：防止索引越界或空指针
    if (ballIndex < 0 || ballIndex >= static_cast<int>(myBalls.size()) || !virus) return;

    Ball* oldBall = myBalls[ballIndex];
    if (!oldBall) return;

    // 🚨 🔥 【核心拦截】：如果子球数量已经达到或超过 16 个
    if (myBalls.size() >= 16) {
        // 直接在内部把刺球给生吞了！将刺球的质量结结实实加给当前撞击的大球
        float virusMass = virus->getMass();
        oldBall->addMass(virusMass);
        return; // ✨ 直接返回，拒绝触发后面的任何分裂逻辑！
    }

    // ========================================================
    // 💥 正常爆炸分裂逻辑（子球数量未满 16）
    // ========================================================
    float originalMass = oldBall->getMass();
    float originalX = oldBall->getX();
    float originalY = oldBall->getY();
    QColor ballColor = oldBall->getColor();

    // 🛡️ 动态保护：本次爆炸最多能产生的总碎片数，确保分裂后总数绝对不超 16
    int maxAllowedNewPieces = 16 - static_cast<int>(myBalls.size());
    int maxPieces = std::min(10, maxAllowedNewPieces + 1); // 最多炸出 10 个

    // 🎯 ✨【核心修改 1】：设定炸刺的基础惩罚时间
    // 既然你设定 2 秒，那我们给被炸出来的碎球和留下的母球统一硬灌 2 秒 CD！
    float punishCooldown = 2.0f;

    // 🎯 ✨【核心修改 2】：随机数量 5 - 8 个碎球
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> randPieceCount(5, 8); // 随机生成 5-8 颗球
    int pieceCount = randPieceCount(gen);

    // 如果原球由于质量或者 16 颗球的上限限制，无法炸出这么多，做一层裁剪防御
    if (pieceCount > maxPieces) pieceCount = maxPieces;

    // 如果大球本身质量太小，不够分裂成两颗球，也直接当食物吃掉
    float minPieceMass = originalMass / pieceCount;
    if (pieceCount < 2 || minPieceMass < 100.0f) { // 加一层低保质量防碎成渣
        oldBall->addMass(virus->getMass());
        return;
    }

    // 质量守恒：原球体积瞬间缩水
    float newMass = originalMass / pieceCount;
    oldBall->setMass(newMass);

    // ⚡ 【致命 BUG 修复】：母球在炸裂之后，必须立刻被重新灌入冷却 CD！
    oldBall->setMergeCooldown(punishCooldown);

    // 随机一个初始偏角，让每次爆炸的碎球方向不僵硬
    std::uniform_real_distribution<float> randAngle(0.0f, 2.0f * 3.1415926f);
    float startAngleOffset = randAngle(gen);

    float explodeForce = 850.0f; // 初始弹射速度

    // 环形生成其余的 pieceCount - 1 个碎球并注入冲力
    for (int i = 1; i < pieceCount; ++i) {
        float angle = startAngleOffset + (i * (2.0f * 3.1415926f) / (pieceCount - 1));

        // 算出初始弹射惯性速度
        float pVx = std::cos(angle) * explodeForce;
        float pVy = std::sin(angle) * explodeForce;

        // 让碎球出生在原球外围，配合我们写好的同体排斥，手感更佳
        float pushDist = oldBall->getRadius() * 1.2f;
        float spawnX = originalX + std::cos(angle) * pushDist;
        float spawnY = originalY + std::sin(angle) * pushDist;

        // 统一创建具备物理位移和控制能力的 ControllableBall
        ControllableBall* newPiece = new ControllableBall(spawnX, spawnY, newMass, this->initSpeed, ballColor);

        // ⚡ 【致命 BUG 修复】：爆出来的每一颗新子球，一出生必须强行焊死 2 秒 CD！
        newPiece->setMergeCooldown(punishCooldown);

        // 💥 注入瞬间冲力速度
        newPiece->applyExplosionPush(pVx, pVy);

        // 稳稳塞进大管家自己的容器里
        myBalls.push_back(newPiece);
    }
}
// 1. 分裂逻辑
void BasePlayer::split(float targetX, float targetY)
{
    OperateManager::split(this, targetX, targetY);
}

// 2. 🎯 ✨【彻底清爽】：直接一行透传，看着极度舒适！
void BasePlayer::merge(float deltaTime)
{
    // 🎯 直接调用我们新写的、完全适配公开接口的合体判定函数
    OperateManager::checkAndMerge(this, deltaTime);
}
void BasePlayer::eject(float targetX, float targetY, std::vector<EjectedMass*>& masses)
{
    OperateManager::eject(this,targetX,targetY,masses);
}