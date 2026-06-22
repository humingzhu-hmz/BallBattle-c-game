// main.cpp
#include <QApplication>
#include "Main.h"
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 实例化专门的游戏对局界面
    GameScene gameWindow;
    gameWindow.show();

    return app.exec();
}