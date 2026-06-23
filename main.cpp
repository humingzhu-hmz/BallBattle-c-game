// main.cpp
#include <QApplication>
#include "Main.h"
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);


    GameScene gameWindow;
    gameWindow.show();

    return app.exec();
}