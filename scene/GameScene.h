#pragma once
#include <QWidget>
#include <QTimer>
#include <vector>

// 引入大管家配置
#include "../map/GameMap.h"
#include "../map/Camera.h"
#include "../player/Player.h"
#include "../entity/FoodManager.h"
#include "../entity/VirusManager.h"
#include "../entity/AIManager.h"
#include "../collision/SpatialGrid.h"
// 结构体声明
struct BackgroundStar {
    float worldX;
    float worldY;
    float radius;
    int alpha;
};

// 【核心修复】：显式暴露出 Scene 基类，确保 main.cpp 能够完美识别
class Scene : public QWidget {
    Q_OBJECT
public:
    explicit Scene(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual ~Scene() = default;
};

// 具体的游戏对局界面
class GameScene : public Scene {
    Q_OBJECT
private:
    GameMap gameMap;
    Player player;
    AIManager aiManager;
    Camera camera;
    QTimer* gameTimer;
    FoodManager foodManager;
    VirusManager virusManager;
    SpatialGrid spatialGrid;
    float mouseScreenX;
    float mouseScreenY;
    float mapw,maph,scrw,scrh;
    float currentScale;
    float halfW;
    float halfH;
    std::vector<BackgroundStar> stars;
    std::vector<Ball*> allLivePlayersBalls;
public:
    explicit GameScene(QWidget *parent = nullptr);
    ~GameScene();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void drawVirusAndPlayer(float camx,float camy,float currentScale,QPainter& painter);
    void gameLoop();
};