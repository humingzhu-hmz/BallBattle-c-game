#pragma once
#include <QSoundEffect>
#include <QObject>

class Music : public QObject {
    Q_OBJECT
private:
    QSoundEffect m_soundEject;
    QSoundEffect m_soundMerge;
    QSoundEffect m_soundRespawn;
    QSoundEffect m_soundGameOver;
    QSoundEffect m_soundSplit;
    QSoundEffect m_soundGameBegin;

    // 把构造私有化，防止外部意外 new 多个实例破坏单例
    Music();
public:
    ~Music() = default;

    static Music& getInstance();

    void playGameBegin();
    void stopGameBegin();
    void playSplit();
    void playEject();
    void playMerge();
    void playRespawn();
    void playGameOver();
};