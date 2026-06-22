#include "GameScene.h"
#include "../collision/CollisionManager.h"
#include "../player/CharacterHandle.h"
#include <QPainter>
#include <QMouseEvent>
#include <random>
GameScene::GameScene(QWidget *parent)
    : Scene(parent),
    gameMap(5000.0f, 4000.0f,1240.0f,880.0f),
    spatialGrid(5000.0f, 4000.0f, 200.0f),
    player(2500.0f, 2000.0f,400.0f,400.0f,2,Qt::red,"Rain"),
    aiManager(3),
    camera(2500.0f, 2000.0f,1.0f),
    mouseScreenX(960.0f),
    mouseScreenY(540.0f),
    foodManager(1000),
    virusManager(30)
{
    mapw=gameMap.getMapWidth();
    maph=gameMap.getMapHeight();
    scrw=gameMap.getscreenwidth();
    scrh=gameMap.getscreenheight();
    halfW=scrw/2;
    halfH=scrh/2;
    qDebug()
        << mapw
        << maph
        << scrw
        << scrh;
    //  视口大小
    setFixedSize(scrw,scrh);
    setWindowTitle("Ballbattle - 游戏对局主界面");
    setMouseTracking(true);

    gameMap.init();

    // 在大地图以及四周 5% 的过渡带内生成少量星光点缀
    std::mt19937 rng(1314);
    std::uniform_real_distribution<float> distX(-200.0f, 5200.0f);
    std::uniform_real_distribution<float> distY(-200.0f, 5200.0f);
    std::uniform_real_distribution<float> distSize(3.0f, 5.0f);
    std::uniform_int_distribution<int> distAlpha(150,240);

    for (int i = 0; i < 500; ++i) {
        stars.push_back({distX(rng), distY(rng), distSize(rng), distAlpha(rng)});
    }

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameScene::gameLoop);
    gameTimer->start(16); // 60FPS 主循环
}
GameScene::~GameScene() {
    delete gameTimer;
}

void GameScene::mouseMoveEvent(QMouseEvent *event) {
    // 完美的 Qt 6 标准写法，警告立刻消失
    mouseScreenX = static_cast<float>(event->position().x());
    mouseScreenY = static_cast<float>(event->position().y());
    // qDebug()
    //     << mouseScreenX
    //     << mouseScreenY;
}
void GameScene::drawVirusAndPlayer(float camX,float camY,float currentScale,QPainter& painter)
{
    // 3. ✨【统一收集】：利用已在头文件中声明好的 allLivePlayersBalls，避免每帧分配内存造成的卡顿

    allLivePlayersBalls.clear();
    // 收集人类玩家的子球
    for (Ball* b : player.getBalls()) {
        if (b && b->getMass() > 0) allLivePlayersBalls.push_back(b);
    }
    // 收集所有 AI 玩家的子球
    for (AIPlayer* ai : aiManager.getAIPlayers()) {
        if (!ai) continue;
        for (Ball* b : ai->getBalls()) {
            if (b && b->getMass() > 0) allLivePlayersBalls.push_back(b);
        }
    }

    // 4. 🎯【你的排序精髓】：全场活着的球，先进行一次彻底的“从小到大”大排队
    std::sort(allLivePlayersBalls.begin(), allLivePlayersBalls.end(), [](Ball* a, Ball* b) {
        return a->getRadius() < b->getRadius();
    });
    float virusThresholdRadius=virusManager.getMinRadius();
    // 5. 🔥【你的分段交替绘制】：利用刺球半径作为分水岭（假设刺球半径为固定值，比如 65.0f）
    size_t i=0;
    // 🟢 阶段 A：由于已经排好序，直接顺着画，所有【比刺球小】的球会完美从小到大画出来
    for (; i < allLivePlayersBalls.size(); ++i) {
        if (allLivePlayersBalls[i]->getRadius() >= virusThresholdRadius) {
            break; // 撞到分水岭，大球留着后面画，暂停循环！
        }

        // 恢复之前因为排序丢失的 QPainter 状态（在画球前，必须让画笔换上它主人的颜色）
        // 假设你的 Ball 内部能追溯到所属的 BasePlayer，或者我们还是用前面提到的给 QPainter 换色
        // 如果你的 Ball::draw 内部能自己处理，这里直接调用：
        allLivePlayersBalls[i]->draw(camX, camY, width(), height(), currentScale, painter);
    }

    // 🟢 阶段 B：把刺球整齐地盖在小球上面。这时候，所有比刺球小的球就被完美“藏”在下面了
    virusManager.draw(camX, camY, width(), height(), currentScale, painter);

    // 🟢 阶段 C：接着刚才刹车的位置，把剩下的【比刺球大】的球全部画完。它们会完美压住刺球
    for (; i < allLivePlayersBalls.size(); ++i) {
        allLivePlayersBalls[i]->draw(camX, camY, width(), height(), currentScale, painter);
    }
}
void GameScene::gameLoop() {
    // qDebug()<<player.getCenterX()<<
    //     player.getCenterY();
    // qDebug()<<camera.getX()<<camera.getY();


    // 【手写纯数学转换】：屏幕坐标转世界坐标
    if (player.getlifeCount() <= 0) {
        // 让游戏停在这一帧，只允许 paintEvent 刷新静态的 GAME OVER 画面
        return;
    }
    currentScale = camera.getScale();
    mouseWorldX = (mouseScreenX - halfW) / currentScale + camera.getX();
    mouseWorldY = (mouseScreenY - halfH) / currentScale + camera.getY();

    // 1. 玩家和 AI 更新
    player.updatePlayer(0.016f, mouseWorldX, mouseWorldY, mapw, maph);
    aiManager.updateAllAI(0.016f, foodManager, mapw, maph);
    foodManager.update(mapw, maph);
    virusManager.update(mapw, maph);
    //更新袍子
    ejectedMassManager.update(0.016f, mapw, maph);
    // 设置目标点为玩家中心
    camera.setTarget(player.getCenterX(), player.getCenterY());

    // ✨ 核心修复 2：将实时窗口宽高 (currentWidth, currentHeight) 灌给相机
    camera.update(
        0.016f,
        mapw,
        maph,
        scrw,   // 替换掉死板的 scrw
        scrh,  // 替换掉死板的 scrh
        player.getMaxBallMass()
        );
    // ========================================================
    // 🎯 核心插入点：在移动后，立刻执行同队子球物理推挤排斥！
    // ========================================================
    // 无论是人类玩家还是 AI，刚炸出来的子球或者在鼠标吸力下聚拢的子球，
    // 在这里由于距离小于半径之和，会被强行推开并互相施加对抗动量，绝对无法重叠成一个点！


    // 2. 统一由边界裁剪官处理卡位
    CollisionManager::checkMapBoundaries(player, mapw, maph);
    for (AIPlayer* ai : aiManager.getAIPlayers()) {
        CollisionManager::checkMapBoundaries(*ai, mapw, maph);
    }

    // ========================================================
    // 3. 统一碰撞清算（基于空间网格的极速模式）
    // ========================================================

    // 【步骤 1】：清空并重建网格索引（每一帧都必须重置）
    spatialGrid.clear();

    // 汇总所有玩家
    std::vector<BasePlayer*> allPlayers;
    allPlayers.push_back(&player); // 人类玩家
    for (AIPlayer* ai : aiManager.getAIPlayers()) {
        if (ai) allPlayers.push_back(ai);
    }
    // 🎯 ✨【在这里接入】：让所有人在进入下一轮碰撞和网格索引前，统一进行质量自然衰减
    for (BasePlayer* p : allPlayers) {
        if (p) {
            p->updateMassDecay(0.016f); // 无论是人类还是AI，百万质量在这一步都会被非线性抽干！
            p->merge(0.016f);
        }
    }
    // 将所有玩家【已经经过物理推挤、完美错开不重叠】的子球填入网格
    for (BasePlayer* p : allPlayers) {
        for (Ball* ball : p->getBalls()) {
            spatialGrid.insert(ball, p);
        }
    }
    // 将所有食物填入网格
    for (Food* food : foodManager.getFoods()) {
        spatialGrid.insert(food, nullptr); // 食物没有 Owner，传 nullptr
    }

    // 将所有刺球填入网格
    for (Virus* v : virusManager.getViruses()) {
        spatialGrid.insert(v, nullptr); // 刺球也没有 Owner
    }

    // ✨ 新增：将所有玩家吐出来的孢子也填入空间网格
    for (EjectedMass* mass : ejectedMassManager.getMasses()) {
        if (mass) {
            spatialGrid.insert(mass, nullptr); // 孢子也是公共物体，Owner 传 nullptr
        }
    }
    // 【步骤 2】：执行碰撞检测（使用网格引擎）

    // 1. 人类玩家的碰撞检测
    // qDebug()<<"1";
    CollisionManager::checkPlayerVsFood(player, foodManager, spatialGrid);
    // qDebug()<<"2";
    CollisionManager::checkPlayerVsVirus(player, virusManager, spatialGrid); // 撞击发生时触发 explodeBall

    CollisionManager::checkPlayerVsEjectedMass(player, ejectedMassManager, spatialGrid);

    for (AIPlayer* ai : aiManager.getAIPlayers()) {
        if (ai) CollisionManager::checkPlayerVsEjectedMass(*ai, ejectedMassManager, spatialGrid);
    }
    // 2. AI 的碰撞检测（含 AI 吃食物、撞刺、以及 AI 之间的互吃）
    for (AIPlayer* ai : aiManager.getAIPlayers()) {
        if (!ai) continue;
        CollisionManager::checkPlayerVsFood(*ai, foodManager, spatialGrid);
        CollisionManager::checkPlayerVsVirus(*ai, virusManager, spatialGrid);
    }
    // qDebug()<<"3";
    // 3. 全场玩家互吃（包含玩家吃 AI，AI 吃 AI）
    CollisionManager::checkAllPlayerCollisions(allPlayers, spatialGrid);
    // qDebug()<<"4";
    //qDebug() << "player myball size"<<player.getBalls().size();
    update();
}
void GameScene::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Space)
    {
        //qDebug() << "split"<<player.getSumMass()<<player.getBalls().size();
        player.split(mouseWorldX,mouseWorldY);
        //qDebug() << "after split"<<player.getSumMass()<<player.getBalls().size();
    }
    if(event->key() == Qt::Key_W)
    {
        //qDebug() << "eject"<<player.getSumMass()<<player.getBalls().size();
        player.eject(mouseWorldX,mouseWorldY,ejectedMassManager.getMasses());
        //qDebug() << "after eject"<<player.getSumMass()<<player.getBalls().size();
    }
}
void GameScene::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::RightButton)
    {
        //qDebug() << "split"<<player.getSumMass()<<player.getBalls().size();
        player.split(mouseWorldX,mouseWorldY);
        //qDebug() << "after split"<<player.getSumMass()<<player.getBalls().size();
    }

}
void GameScene::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    float camX = camera.getX();
    float camY = camera.getY();

    // ✨ 确保使用的是当前最新计算出来的缩放比
    //currentScale = camera.getScale();

    // A. 视口底层深空纯黑
    painter.fillRect(rect(), Qt::black);

    // B. 绘制过渡带星光点缀
    painter.setPen(Qt::NoPen);
    for (const auto& star : stars) {
        // 🎯【核心修复】：世界转屏幕公式必须先应用缩放，再平移到屏幕中心！
        float scrX = (star.worldX - camX) * currentScale + halfW;
        float scrY = (star.worldY - camY) * currentScale + halfH;

        if (scrX >= 0 && scrX <= width() && scrY >= 0 && scrY <= height()) {
            // 星光的半径也应该随着镜头拉远而变小
            float scrRadius = star.radius * currentScale;
            painter.setBrush(QColor(255, 255, 255, star.alpha));
            painter.drawEllipse(QPointF(scrX, scrY), scrRadius, scrRadius);
        }
    }

    // C. 绘制主体（世界坐标系，受 currentScale 变焦影响）
    gameMap.draw(camX, camY, scrw, scrh, currentScale, painter);
    foodManager.draw(camX, camY, scrw, scrh, currentScale, painter);

    // 1️⃣ 先绘制刺球与玩家子球的身体
    drawVirusAndPlayer(camX, camY, currentScale, painter);

    // 🎯 ✨【文字管理器切入点 1：球心名字渲染】
    // 在这里调用，因为此时绘制矩阵/状态属于主体层。
    // 注意：由于你的 drawBallText 内部直接读了 mainBall->getX() 世界坐标，
    // 为了让名字完美对齐屏幕上的球，我们需要在调用前让 painter 应用相机的变换矩阵，
    // 或者如果你更习惯在 CharacterHandle 内部做转换，直接把画笔交给它。
    // 这里我们先save，然后平移缩放画笔，让坐标轴跟世界坐标重合，这样 drawBallText 内部直接画世界坐标就百分百精准了！
    painter.save();
    painter.translate(halfW, halfH); // 平移到屏幕中心
    painter.scale(currentScale, currentScale); // 缩放视野
    painter.translate(-camX, -camY); // 锚定相机世界位置

    // 传入你的玩家对象指针（假设对象或指针叫 &player 或 player 指针，根据你的具体定义调整）
    CharacterHandle::drawBallText(painter, &player);
    // 如果你有 AI 玩家管理器，也可以在此处遍历 AI 绘制名字：
    // for(auto* ai : aiManager.getAIPlayers()) { CharacterHandle::drawBallText(painter, ai); }
    painter.restore();

    ejectedMassManager.draw(camX, camY, scrw, scrh, currentScale, painter);

    // D. 绘制 UI 固顶层（屏幕坐标系，不受 currentScale 和相机平移影响）

    // 2️⃣ ✨【文字管理器切入点 2：屏幕左上角状态看板】
    // 它属于屏幕绝对UI，不随镜头放大缩小，直接传屏幕的宽和高
    CharacterHandle::drawScreenHUD(painter, &player, width(), height());

    // E. 右上角 HUD 小地图强制置顶渲染（保持不变，小地图不需要变焦）
    float miniMapSize = 180.0f;
    float miniMapX = width() - miniMapSize - 40.0f;
    float miniMapY = 40.0f;

    painter.setPen(QPen(Qt::black, 3));
    painter.setBrush(QColor(40, 40, 40, 220));
    painter.drawRect(QRectF(miniMapX, miniMapY, miniMapSize, miniMapSize));

    float pX = player.getCenterX();
    float pY = player.getCenterY();
    float scaleX = miniMapSize / mapw;
    float scaleY = miniMapSize / maph;
    float playerMiniX = miniMapX + (pX * scaleX);
    float playerMiniY = miniMapY + (pY * scaleY);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::green);
    painter.drawEllipse(QPointF(playerMiniX, playerMiniY), 5.0f, 5.0f);

    // 3️⃣ ✨【文字管理器切入点 3：游戏结束大结算提示】
    // 必须放在最后渲染，确保它的半透明红黑色滤镜能把背后的地图、小地图全部盖住，实现完美的“全屏霸屏宣告”
    if (!player.getlifeStatue() || player.getlifeCount() <= 0) {
        CharacterHandle::drawGameOverSummary(painter, &player, width(), height());
    }
}