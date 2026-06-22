// map/Camera.h
#pragma once

class Camera {
private:
    static float x;
    static float y;
    static float targetX;
    static float targetY;
    float lerpSpeed;
    //Linear Interpolation Speed（线性插值速度 / 缓动速度)
    //🔍 没加 lerpSpeed 时：生硬的死板跟随如果不使用 lerpSpeed，
    // 相机的坐标强行等于玩家的坐标（x = targetX; y = targetY;）：
    // 当玩家按分裂键，球体瞬间向前弹射出几百码时，相机也会在一帧之内瞬间瞬移过去。视觉效果：
    // 画面会产生极其剧烈的机械式抖动，玩家看久了甚至会头晕想吐。🚀 加了 lerpSpeed 后：
    // 丝滑的电影镜头感我们在 Camera::update 里写的核心公式是：
    // x += (targetX - x) * lerpSpeed * deltaTime;
    // 这个公式在数学和物理上极其优雅，我们可以把它拆开来看：(targetX - x)
    // 代表目标点（玩家）和相机当前位置之间的距离差。* lerpSpeed * deltaTime：
    // 代表相机在这一帧，只往前挪移这个距离差的固定比例（比如乘以 $5.0 \times 0.016$ 秒，大约每帧挪移当前差距的 $8\%$）。
    // ========================================================
    // ✨【核心升级：独立相机的缩放控制变量】
    // ========================================================
    float currentScale;       // 每一帧正在生效的真实渲染缩放比 (即 zoom)
    float targetScale;        // 经过高阶公式算出来的目标缩放比
    float lastTriggerMass;    // 临界死区控制：上一次引发视野变动的质量基准
    // scale 屏幕单位距离/真实单位距离 ,scale=0.1,也就是把真实世界缩小十倍显示
public:
    Camera(float startX, float startY,float currentScale=1.0f);// 初始化默认 1:1 缩放

    static void setPosition(float newX, float newY);
    void setTarget(float tX, float tY);

    // 【升级参数】：不仅传入地图和屏幕尺寸，同时把玩家当前的总质量灌进来
    void update(float deltaTime, float mapWidth, float mapHeight, float screenWidth, float screenHeight, float currentTotalMass);

    float getX() const;
    float getY() const;

    // ✨【新增接口】：供外部所有绘制实体 (Ball, Food, Virus) 统一获取当前的缩放比例
    float getScale() const;
};