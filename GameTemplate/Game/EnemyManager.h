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
    void CountUpDeathCount() {
        m_deathCount++;
	}
    int GetDeathCount() const {
        return m_deathCount;
	}
private:
    float m_spawnTimer = 0.0f;
	int m_deathCount = 0; // “|‚µ‚½“G‚Ì”‚ğƒJƒEƒ“ƒg‚·‚é•Ï”
};