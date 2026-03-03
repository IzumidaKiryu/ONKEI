#pragma once


// 1粒のパーティクル構造体
struct ParticleData {
    nsK2Engine::SpriteRender sprite;
    Vector3 velocity; // 飛んでいく方向と速さ
    Vector4 baseColor; // ★追加: 元の色を覚えておく！
};

class HitEffect : public IGameObject
{
public:
    HitEffect();
    ~HitEffect();

    // 発生させる場所を指定して初期化
    void Init(Vector3 pos);

    void Update() override;
    void Render(RenderContext& rc) override;

private:
    static const int PARTICLE_NUM = 6; // 飛び散る粒の数
    ParticleData m_particles[PARTICLE_NUM];

    float m_lifeTimer = 0.0f; // エフェクトの寿命管理
};