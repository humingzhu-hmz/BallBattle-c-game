

```markdown
# 🎮 Ballbattle — 基于 Qt 6 的高性能多玩家吞噬对战游戏

![C++](https://img.shields.io/badge/Language-C%2B%2B11%2B-blue.svg)
![Qt](https://img.shields.io/badge/Framework-Qt_6.x-green.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey.svg)

`Ballbattle` 是一款使用 C++ 11 与 Qt 6 框架纯手写的、高动态物理吞噬对抗游戏（类似球球大作战/大鱼吃小鱼）。项目拒绝臃肿的传统循环，自研**九宫格空间网格索引**与**高阶幂次速度控制算法**，保障百级实体同屏清算依然丝滑 60FPS！

---

## ✨ 核心特性亮点

* 🎛️ **无键平滑追踪**：开启 `setMouseTracking(true)` 实时捕捉鼠标世界坐标，提供无延迟、无阻尼的丝滑移动操控。
* 🤖 **智能雷达 AI**：内置 8 个独立决策智能体（AI 玩家），具备自动寻找食物、规避巨型刺球（Virus）以及追击低质量玩家的动态局势分析机制。
* 🌌 **穿梭氛围视差**：在 5000 × 4000 的大地图中通过随机数渲染 500 颗不同暗亮度的星点背景，跟随相机移动产生丰富的空间层次视觉差。
* 🧱 **防坍缩同队推挤**：独创 `checkSelfPushback` 几何纠偏，分裂多球聚合时根据质量反比平滑平开坐标，绝不发生重叠死锁与肉体坍缩。
* 📐 **智能边界裁剪**：针对质量过万（如 17000+）的超级巨球进行中心点弹性裁剪，完美破除传统“半径撞墙算法”导致的边缘死锁倒退 Bug。
* 🎥 **智能视口相机**：独立的 `Camera` 控制器锁死玩家质心，视口随大球当前质量自动完成平滑的镜头拉高与缩小（Scale）。

---

## 🛠️ 核心硬核算法设计

### 1. 二维空间网格引擎 (`SpatialGrid`)
传统碰撞清算两两比对的时间复杂度为 $O(N^2)$，当全场存在 1000+ 食物和数十球体时，CPU 会瞬间卡死。
* **解决方案**：将地图均匀切割为 200 × 200 像素的小网格。每帧清空重建索引。
* **清算优化**：碰撞清算时，任何实体仅需与其当前所在网格及**周围九宫格邻居**进行欧氏距离比对，直接将复杂度降低到接近 $O(N)$。

### 2. 高阶幂次速度阻尼与死区缓动
为了兼顾“小球灵动敏捷，大球沉稳迟钝”的手感：
* 利用 `std::exp` 和 `std::pow` 结合质量（`Mass`）算得速度随体积膨胀平滑逼近保底速 `200.0f`。
* 引入 30 像素微观死区限制，球体在极近距离时自动减速缓冲，防止在鼠标指针中心发生高频抖动。

---

## 📂 项目模块解耦

```text
Ballbattle/
├── Ball                  # 面向对象多态物理底层基类 (坐标/质量/半径/颜色)
│   ├── ControllableBall  # 玩家/AI 动作控制、炸刺弹射冲力融合实体
│   ├── Food              # 营养食物静态碰撞体
│   ├── Virus             # 巨型分裂危险刺球
│   └── EjectedMass       # 喷吐生成的质量孢子
├── BasePlayer            # 实体控制容器 (管理 std::vector<Ball*>)
│   ├── Player            # 人类玩家行为分发
│   └── AIPlayer          # 智能体雷达决策控制器
├── SpatialGrid           # 自研九宫格空间定位引擎
├── Camera                # 视觉平滑轴心跟随相机
├── CollisionManager      # 全局高频碰撞清算与物理位置纠偏官
└── GameScene             # 游戏总舞台，由 QTimer (16ms) 驱动的大心脏

```

---

## 🔄 每帧生命周期拓扑 (16ms / 60FPS)

```text
main()
  └── GameScene 初始化启动
        └── [QTimer 触发] GameScene::gameLoop()
              ├── 1. 坐标双向转换 (屏幕鼠标坐标 + Camera 坐标换算 ──> 世界绝对坐标)
              ├── 2. 状态平滑更新 (Player 常规位移更新 & AIManager 智能体环境决策)
              ├── 3. 物理位置纠偏 (checkSelfPushback 纠偏 + checkMapBoundaries 边界裁剪)
              ├── 4. 重建空间索引 (清空旧网格 ──> 全场活体重新插入 SpatialGrid 网格)
              ├── 5. 九宫格极速清算 (吃食物增重、撞刺球炸裂分裂多球、玩家间大吃小清算)
              └── 6. 界面局部重绘 (update() ──> paintEvent() 结合相机视野渲染)

```

---

## 🚀 快速启动

### 环境依赖

* **Compiler**: 支持 C++ 11 及以上的编译器 (GCC / Clang / MSVC)
* **Framework**: Qt 6.x (兼容高级别 Qt 5)
* **Build Tool**: CMake 或 Qt Creator 默认的 qmake

### 编译运行

1. 克隆本项目：
```bash
git clone https://github.com/humingzhu-hmz/BallBattle-c-game.git

```


2. 使用 **Qt Creator** 打开项目根目录下的 `CMakeLists.txt` 或 `Ballbattle.pro`。
3. 选择对应的 Kits 构建套件（如 Desktop Qt 6.x MinGW / MSVC）。
4. 点击左下角 **Run (Ctrl + R)** 即可一键编译并进入游戏！

---

## 📝 开发者碎碎念 (数据驱动调试)

开发过程中遇到过非常诡异的“球离远了不过来，靠近鼠标反而倒退”的死锁 Bug。
通过在物理更新周期内实时打印 `Mass/Dist/Speed` 物理变量进行**数据驱动排查**，最终发现是同队子球排斥逻辑中误灌注了速度冲力导致的。将排斥逻辑重构为“纯几何位置挪移，不干扰物理速度”，并将边界硬撞墙改为“中心点安全裁剪”后，游戏控制权百分之百恢复丝滑。写游戏，数据永远比瞎猜更有用！

```

```
