#include "stdafx.h"
#include "HitEffect.h"

// ランダム関数マクロ
#define RANDOM_FLOAT(min, max) ((min) + ((float)rand() / RAND_MAX) * ((max) - (min)))

HitEffect::HitEffect() {}
HitEffect::~HitEffect() {}

void HitEffect::Init(Vector3 pos)
{
    for (int i = 0; i < PARTICLE_NUM; i++) {
        // 画像読み込み (手持ちのキラキラ画像や、dot.DDSでもOK)
        m_particles[i].sprite.Init("Assets/sprite/hosi1.DDS", 50.0f, 50.0f);

        // 発生位置（ノーツを叩いた場所）
        m_particles[i].sprite.SetPosition(pos);

        // 加算合成（光る表現）ができるならセット
        // m_particles[i].sprite.SetBlendMode(BlendMode::Add); 

        // ★ランダムに飛び散る速度を決める
        // 360度ランダムな方向に飛ばす
        float angle = RANDOM_FLOAT(0.0f, 6.28f); // 0 ~ 2π (ラジアン)
        float speed = RANDOM_FLOAT(300.0f, 600.0f); // 弾ける強さ

        m_particles[i].velocity.x = cosf(angle) * speed;
        m_particles[i].velocity.y = sinf(angle) * speed;
        m_particles[i].velocity.z = 0.0f;

        // 色を少しランダムにする（黄色〜白）
        // ★修正: 計算した色を変数に入れる
        Vector4 color = Vector4(1.0f, 1.0f, RANDOM_FLOAT(0.5f, 1.0f), 1.0f);

        // ★修正: 色を保存して、セットする
        m_particles[i].baseColor = color;
        m_particles[i].sprite.SetMulColor(Vector4(1.0f, 1.0f, RANDOM_FLOAT(0.5f, 1.0f), 1.0f));
    }

    m_lifeTimer = 0.5f; // 0.5秒で消える
}

void HitEffect::Update()
{
    float dt = g_gameTime->GetFrameDeltaTime();
    m_lifeTimer -= dt;

    // 寿命が尽きたら自分自身を削除
    if (m_lifeTimer <= 0.0f) {
        DeleteGO(this);
        return;
    }

    // 透明度の計算 (残り時間 / 全体時間) -> だんだん透明に
    float alpha = m_lifeTimer / 0.5f;

    for (int i = 0; i < PARTICLE_NUM; i++) {
        // 1. 移動させる
        Vector3 pos = m_particles[i].sprite.GetPosition();
        pos += m_particles[i].velocity * dt;
        m_particles[i].sprite.SetPosition(pos);

        // 2. 減速させる（摩擦）
        // これを入れると「パッ！と弾けてフワッと止まる」感じになります
        m_particles[i].velocity *= 0.90f;

        // 3. 回転させる（お好みで）
        // m_particles[i].sprite.SetRotation(...);

        // 4. 透明度適用

     // ★修正: GetMulColor() は使わない！
        // 保存しておいた「元の色」を取り出す
        Vector4 color = m_particles[i].baseColor;
        color.w = alpha;
        m_particles[i].sprite.SetMulColor(color);

        m_particles[i].sprite.Update();
    }
}

void HitEffect::Render(RenderContext& rc)
{
    for (int i = 0; i < PARTICLE_NUM; i++) {
        m_particles[i].sprite.Draw(rc);
    }
}