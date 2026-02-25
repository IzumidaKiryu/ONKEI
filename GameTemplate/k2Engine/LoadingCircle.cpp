//#include "stdafx.h"
#include "k2EnginePreCompile.h"
#include "LoadingCircle.h"


using namespace nsK2Engine;
// HLSLのsmoothstepと同じ動きをする関数
float SmoothStep(float edge0, float edge1, float x)
{
    float t = (x - edge0) / (edge1 - edge0);
    // 0.0 ～ 1.0 に制限 (clamp)
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    // エルミート補間
    return t * t * (3.0f - 2.0f * t);
}

void LoadingCircle::Init(const Vector3& centerPos)
{
    // --- 1. 色の定義 (虹ヶ咲っぽい9色を定義) ---
    // 順番は上(0番)から時計回りに設定します
    m_colors = {
        Vector4(1.0f, 0.6f, 0.7f, 1.0f), // 1. 上: ピンク (歩夢)
        Vector4(1.0f, 1.0f, 0.6f, 1.0f), // 2. 右上: 黄 (かすみ)
        Vector4(0.6f, 0.8f, 1.0f, 1.0f), // 3. 右: 水色 (しずく)
        Vector4(0.3f, 0.3f, 0.9f, 1.0f), // 4. 右下: 青 (果林)
        Vector4(1.0f, 0.5f, 0.2f, 1.0f), // 5. 下: オレンジ (愛)
        Vector4(0.9f, 0.6f, 0.9f, 1.0f), // 6. 左下: すみれ色 (彼方)
        Vector4(1.0f, 0.2f, 0.2f, 1.0f), // 7. 左: 赤 (せつ菜)
        Vector4(0.6f, 1.0f, 0.6f, 1.0f), // 8. 左上: 緑 (エマ)
        Vector4(0.8f, 0.8f, 0.8f, 1.0f), // 9. 最後の枠: グレーor白 (璃奈)
    };

    // --- 2. 配置 ---
    float radius = 40.0f; // 円の半径

    for (int i = 0; i < COUNT; i++)
    {
        // 白いV字画像を読み込む
        m_sprites[i].Init("Assets/sprite/v.DDS", 20.0f, 20.0f); // サイズは適宜調整

        // 角度計算 (上を0度として時計回り)
        // 360度 / 9個 = 40度ずつ
        // 0番目が「上(90度)」に来るように調整
        float angleDeg = -40.0f * i;

        // ラジアン変換
        float rad = Math::DegToRad(angleDeg + 90.0f); // +90して12時スタートにする

        // 位置計算 (cos, sin)
        Vector3 pos;
        pos.x = centerPos.x + radius * cosf(rad);
        pos.y = centerPos.y + radius * sinf(rad);
        pos.z = 0.0f;
        m_sprites[i].SetPosition(pos);

        // 向き計算 (V字の底が中心を向くように)
        // 画像のV字が「上向き」で描かれている前提
        Quaternion rot;
        // そのままの角度だと外を向くので、+180度して中心を向かせる、などの調整が必要かも
        rot.SetRotationDeg(Vector3::AxisZ, angleDeg);
        m_sprites[i].SetRotation(rot);

        // 初期カラー設定 (色は固定！)
        m_sprites[i].SetMulColor(m_colors[i]);
    }
}

void LoadingCircle::Update()
{
    // --- 光のアニメーション ---
    float speed = 12.0f;
    float time = g_gameTime->GetTotalTime() * speed;

    // 現在の「光の中心」の位置 (0.0 ～ 9.0)
    float activeIndex = fmod(time, (float)COUNT);

    // 非アクティブ時の色（灰色）
    // 少し暗めのグレーにすると、光った時との対比で綺麗に見えます
    Vector4 grayColor(0.3f, 0.3f, 0.3f, 1.0f);

    for (int i = 0; i < COUNT; i++)
    {
        // 距離計算（前回と同じ）
        float dist = abs(activeIndex - (float)i);
        if (dist > COUNT * 0.5f) {
            dist = COUNT - dist;
        }

        // 光の強さ (Intensity) を計算
        // 0.0(暗い) ～ 1.0(明るい)
        // 距離が近いほど 1.0 に近づく
        float intensity = 1.0f - SmoothStep(0.0f, 2.5f, dist);

        // ★ここが変更点！
        // 「グレー」と「メンバーカラー」を intensity で混ぜる
        // intensity が 0 ならグレー、1 ならメンバーカラー
        Vector4 finalColor;

        // 線形補間 (Lerp)
        // finalColor = grayColor * (1.0f - intensity) + m_colors[i] * intensity;
        finalColor = grayColor * (1.0f - intensity) + m_colors[i] * intensity;

        // 必要ならアルファ値も調整（完全に消したくない場合は 1.0 のままでOK）
        // 少し透過させたいなら以下のように設定
        // finalColor.w = 0.5f + 0.5f * intensity; // グレーの時は半透明、光ると不透明

        m_sprites[i].SetMulColor(finalColor);
        m_sprites[i].Update();
    }
}

void LoadingCircle::Draw(RenderContext& rc)
{
    for (int i = 0; i < COUNT; i++)
    {
        m_sprites[i].Draw(rc);
    }
}