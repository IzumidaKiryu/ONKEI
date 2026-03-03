#pragma once
#include "Enemy.h"
class EnemyManager : public IGameObject {
public:
    void Init();
    void Update() override {
        m_spawnTimer += g_gameTime->GetFrameDeltaTime();
        if (m_spawnTimer > 5.0f) { // 5•b‚É1‰ñ¶¬
            NewGO<Enemy>(0);
            m_spawnTimer = 0.0f;
        }
    }
private:
    float m_spawnTimer = 0.0f;
};