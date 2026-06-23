#pragma once
#include <vector>
#include "Ball.h" // 👈 核心修改：让编译器完整看清父类的模样，直接包含它！
#include <cmath>
#include <cstring>
class EjectedMass;
// ========================================================
// 具备运动天赋的子球
//.
//    大地图中随时存在数千个食物豆豆 (Food) 和几十个绿刺球 (Virus)，它们一辈子待在原地，
//    根本不需要“速度 (speed)”和“跑路逻辑 (update)”。如果直接写在父类 Ball 里面，
//    这几千个静态物体都会白白携带这些变量，导致内存暴涨。
//    现在让基类 Ball 只留纯几何属性 (x, y, radius, mass)，而将运动天赋单独剥离给
//    ControllableBall，做到了全图几千个静态食物的内存零浪费。
//
//
//   把移动逻辑和速度变量收拢在 ControllableBall 内部，只有玩家和 AI 的球每帧跑路时，
//    才会去动态计算自己的体重反比速度 (currentSpeed = baseSpeed / radius)。
//    完美实现了游戏后期大球天然而沉稳的阻尼手感，不需要在外面写恶心的 if(isFood) 判断。
// ========================================================
// controllableball只比ball多性质一个是speed(可以移动),
class ControllableBall : public Ball {
    float speed;
    float pushVx = 0.0f; // 弹射速度 X 分量
    float pushVy = 0.0f; // 弹射速度 Y 分量
    float pushDecay = 0.85f; // 惯性衰减系数（每帧乘以 0.95，直到趋近于0）
public:
    ControllableBall(float startX, float startY, float startMass = 400.0f,float speed=400.0f,QColor color=Qt::red);
    virtual ~ControllableBall() = default;

    // 严谨风格：显示声明 virtual 和 override
    // 这个update只用来更新一个球
    virtual void update(float deltaTime, float targetX, float targetY) override;
    // override:重写,在现代c++规范中,override告诉编译器:"我就是要重写父类虚函数"否则会有警告
    virtual void getSpeed(float& acceptspeed) const override;
    virtual void setSpeed(float  newspeed) override;
    // 给碎球施加向外的冲力
    void applyExplosionPush(float vx, float vy);
};

// ========================================================
// 统领全军的玩家意志/灵魂
// ========================================================

// 📌 为什么 Player 旗下的容器是 std::vector<Ball*> 类似物，而不是 ControllableBall*？
// ───────────────────────────────────────────────────────────────────────────────────────
// 1. 【面向通用接口：一套碰撞函数管全场】
//     碰撞检测函数中的对象是基类ball,所以我们用vector<Ball*>
//    在 C++ 面向对象中，基类指针 (Ball*) 可以安全地指向任何子类对象。
//    全图所有的球（玩家、AI、食物、刺球）都是 Ball 的儿子。如果玩家用 ControllableBall* 容器，
//    那么碰撞系统 (CollisionManager) 在检测大球吃食物、大球吃刺球时，就必须为各种子类组合
//    生硬地重写无数个碰撞函数，完全浪费了多态特性。
//    维持 Ball* 容器，相当于给所有人发了一张统一的“物理世界通行证”，全场碰撞只需要一套
//    checkCollision(Ball& a, Ball& b) 就能完美搞定。
//
// 2. 【指鹿为马：虚函数动态绑定】
//    容器虽然表面上存的是通用的 Ball* 面具，但我们在 new 的时候塞入的是 ControllableBall 实物。
//    当 Player::update 遍历调用 ball->update(...) 时，因为父类声明了 virtual (虚函数)，
//    C++ 运行时会自动越过外层的 Ball 面具，精准触发 ControllableBall 内部写好的越胖越慢跑路逻辑。
//
class BasePlayer {
private:
    std::vector<Ball*> myBalls; // 完美多态容器
    float  centerX;
    float  centerY;
    const float initMass ;
    const float initSpeed;
    float minRadius;
    bool lifeStatue;
    QColor color;// 玩家的颜色
    QString name;
    static int counts;
    void applyMassDecay(float deltaTime);
public:
    BasePlayer(float startX, float startY,float startMass,float speed,QColor color);

    //只要一个类会被用作基类，其析构函数必须声明为 virtual，否则通过基类指针销毁派生类时会导致内存泄漏。
    virtual ~BasePlayer();

    // 用来更新myballs里面的所有球
    void  update(float deltaTime, float targetX, float targetY) ;

    void removeBall(int index);
    void removeBall(Ball* ball);
    float getCenterX() ;
    float getCenterY() ;
    bool getlifeStatue();
    float getSumMass() ;       // 获取当前玩家总质量
    float getMaxBallMass() ;   // 获取当前玩家最大子球质量
    float getMinRadius();
    void  spawnPlayer(float mapWidth,float mapHeight);
    void  draw(float cameraX, float cameraY, float screenWidth, float screenHeight,float scale, QPainter& painter);
    QColor getColor() const;
    std::vector<Ball*>& getBalls();
    // ✨ 新增：所有玩家（真实玩家与AI）共有的质量衰减逻辑
    void updateMassDecay(float deltaTime);
    void split(float targetX,float targetY);
    QString getName() const ;
    void setName(QString name);
    virtual int getlifeCount() =0;
    // 加上 & 符号，改成引用传递
    void eject(float targetX, float targetY, std::vector<EjectedMass*>& masses);
    void merge(float deltaTime);
    void explodeBall(int ballIndex, Ball* virus);
};