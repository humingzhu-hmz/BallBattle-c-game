#include "MusicManager.h"
#include <QUrl>
#include <QCoreApplication>
#include <QDir>

Music::Music() {
    // 🎯 直接写死你电脑上的音频文件夹绝对路径
    QString musicFolderPath = "E:/desktop/Program_and_code/c++ game Ballbattle/Music";


    m_soundEject.setSource(QUrl::fromLocalFile(QDir(musicFolderPath).filePath("eject.wav")));
    m_soundMerge.setSource(QUrl::fromLocalFile(QDir(musicFolderPath).filePath("merge.wav")));
    m_soundRespawn.setSource(QUrl::fromLocalFile(QDir(musicFolderPath).filePath("respawn.wav")));
    m_soundGameOver.setSource(QUrl::fromLocalFile(QDir(musicFolderPath).filePath("gameover.wav")));
    m_soundSplit.setSource(QUrl::fromLocalFile(QDir(musicFolderPath).filePath("split.wav")));
    m_soundGameBegin.setSource(QUrl::fromLocalFile(QDir(musicFolderPath).filePath("gamebegin.wav")));

    // 循环和音量设置保持不变...
    m_soundGameBegin.setLoopCount(QSoundEffect::Infinite);
    m_soundEject.setVolume(1.6);
    m_soundMerge.setVolume(1.5);
    m_soundRespawn.setVolume(2.0f);
    m_soundGameOver.setVolume(1.0f);
    m_soundSplit.setVolume(1.0f);
    m_soundGameBegin.setVolume(0.5f);
}

Music& Music::getInstance() {
    static Music instance;
    return instance;
}

// 播放与停止控制
void Music::playGameBegin() {
    if (m_soundGameBegin.isLoaded() && !m_soundGameBegin.isPlaying()) {
        m_soundGameBegin.play();
    }
}

void Music::stopGameBegin() {
    if (m_soundGameBegin.isPlaying()) {
        m_soundGameBegin.stop(); //
    }
}

void Music::playSplit()    { if (m_soundSplit.isLoaded()) m_soundSplit.play();}
void Music::playEject()    { if (m_soundEject.isLoaded()) m_soundEject.play(); }
void Music::playMerge()    { if (m_soundMerge.isLoaded()) m_soundMerge.play(); }
void Music::playRespawn()  { if (m_soundRespawn.isLoaded()) m_soundRespawn.play(); }
void Music::playGameOver() { if (m_soundGameOver.isLoaded()) m_soundGameOver.play(); }