#pragma once
#include "Enemy.h"
class Enemy;
class EnemyManager : public IGameObject {
public:
    void Init();
    void Update() override {
        m_spawnTimer += g_gameTime->GetFrameDeltaTime();
        //if (m_spawnTimer > 5.0f) { // 5秒に1回生成
        //    NewGO<Enemy>(0);
        //    m_spawnTimer = 0.0f;
        //}
    }
    void CountUpDeathCount() {
        m_deathCount++;
	}
    int GetDeathCount() const {
        return m_deathCount;
	}
	Enemy* m_enemy = nullptr; // 生成した敵を管理するためのポインタ
private:
    float m_spawnTimer = 0.0f;
	int m_deathCount = 0; // 倒した敵の数をカウントする変数

};