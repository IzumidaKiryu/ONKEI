#pragma once
#include <array>
#include "k2EnginePreCompile.h"

using namespace nsK2Engine;

class LoadingCircle
{

public:
    // 初期化 (中心座標を指定)
    void Init(const Vector3& centerPos);

    // 更新 (毎フレーム呼ぶ)
    void Update();

    // 描画
    void Draw(RenderContext& rc);

private:
    // 9個のスプライトを持つ
    static const int COUNT = 9;
    std::array<SpriteRender, COUNT> m_sprites;

    // 各場所の「固定カラー」
    std::array<Vector4, COUNT> m_colors;
};


