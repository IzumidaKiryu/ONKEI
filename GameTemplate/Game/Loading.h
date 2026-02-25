#pragma once
#include "k2EnginePreCompile.h"
#include "LoadingCircle.h"
#include <functional> // 忘れずに！
#include "DiamondWipe.h"

using namespace nsK2Engine;

class Loading : public IGameObject
{
public:
    Loading();
    ~Loading();
        // 引数名をわかりやすく記述
    void Init(std::function<void()> nextSceneCreator,
        std::function<bool()> isReadyChecker,
        bool isAutoDelete = true
    );
    void Update() override;
    void Render(RenderContext& rc) override;

private:

    enum class State { FadeIn, Load, FadeOut };
    State m_state = State::FadeIn;

    DiamondWipe m_diamondWipe;

    // --- UI ---
    SpriteRender m_text;
    SpriteRender m_dots[3];

    LoadingCircle m_loadingCircle;

    std::function<void()> m_nextSceneCreator;
    // ★追加: 「準備できた？」を確認するための関数を保存する変数
    std::function<bool()> m_isReadyChecker;
    IGameObject* m_previousScene = nullptr;

    float m_timer = 0.0f;
    float m_transitionVal = 0.0f;
    bool m_isAutoDelete = true; // 保存用変数
};