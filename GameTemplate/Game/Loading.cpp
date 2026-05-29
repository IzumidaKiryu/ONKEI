#include "stdafx.h"
#include "Loading.h"
#include "DiamondWipe.h"

// 頂点構造体
struct SimpleVertex {
    Vector3 pos;
    Vector2 uv;
};

Loading::Loading() {}

Loading::~Loading() {}

void Loading::Init(std::function<void()> nextSceneCreator,
    std::function<bool()> isReadyChecker,
    bool isAutoDelete)
{
    m_nextSceneCreator = nextSceneCreator;
    // ★追加: チェック関数を保存
    // もし指定がなければ「すぐ終わる(trueを返す)」関数を入れておく
    if (isReadyChecker) {
        m_isReadyChecker = isReadyChecker;
    }
    else {
        m_isReadyChecker = []() { return true; };
    }
    m_isAutoDelete = isAutoDelete; // ★保存！

    m_diamondWipe.Init("Assets/UI/loading.DDS", 1920, 1080);//画像差し替え必須
    m_diamondWipe.SetPosition(Vector3::Zero);

    // UI初期化
    m_loadingCircle.Init(Vector3(800.0f, -380.0f, 0.0f));

    m_text.Init("Assets/sprite/LoadingText.DDS", 300, 60);//画像差し替え必須
    m_text.SetPosition(Vector3(570.0f, -405.0f, 0.0f));

    // ★ドットの初期化と配置
    float dotSize = 7.0f;  // ドットのサイズ（画像のサイズに合わせる）
    float spacing = 12.0f;  // ドット同士の間隔

    // "Now Loading" の画像の右端のX座標を起点にする
    float textWidth = 300.0f; // 画像の幅（ピクセル単位）
    float startX = m_text.GetPosition().x + (textWidth * 0.5f);


    for (int i = 0; i < 3; i++) {
        // 白いドット画像で初期化
        m_dots[i].Init("Assets/sprite/dot.DDS", dotSize, dotSize);//画像差し替え必須

        // 1番目(i=0)は右端から spacing だけ離れた位置
        float dotX = startX + spacing + (i * spacing);

        m_dots[i].SetPosition(Vector3(dotX, m_text.GetPosition().y - 10.0f, 0.0f));

        // 初期状態では全部非表示にしておく
        m_dots[i].SetActive(false);
    }

    m_state = State::FadeIn;
    m_timer = 0.0f;
    m_transitionVal = 0.0f;
}


void Loading::Update()
{
    float deltaTime = g_gameTime->GetFrameDeltaTime();



    // 文字点滅
    float textPulse = 0.7f + 0.3f * sinf(g_gameTime->GetFrameDeltaTime() * 5.0f);
    m_text.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, textPulse));

    // シーン遷移ロジック
    switch (m_state) {
    case State::FadeIn:
        m_timer += deltaTime;
        m_transitionVal = m_timer / 0.7f; // 0.5秒でフェードイン
        if (m_transitionVal >= 1.0f) {
            m_transitionVal = 1.0f;
            m_state = State::Load;
            m_timer = 0.0f;
        }
        break;

    case State::Load:
        // 1. シーン生成 (初回のみ)
         // m_nextSceneCreator を呼ぶと RhythmGame が作られますが、
         // Initが一瞬で終わるので、すぐここに戻ってきます。
        if (m_nextSceneCreator) {
            m_nextSceneCreator();
            m_nextSceneCreator = nullptr; // 二度呼ばないように
        }

        // 2. 準備完了チェック
        // ★修正: RhythmGameへの依存を削除！
        // 保存しておいた「チェック関数」を呼び出して、trueが返ってきたらフェードアウトへ
        if (m_isReadyChecker()) {
            m_state = State::FadeOut;
        }
        // 準備できてないなら、breakして Update を抜ける
        // -> 次のフレームもサークルが回り続ける！
        break;

    case State::FadeOut:
        m_timer += deltaTime;
        m_transitionVal = 1.0f - (m_timer / 0.5f); // 0.5秒でフェードアウト
        if (m_transitionVal <= 0.0f) {
            m_transitionVal = 0.0f;

            if (m_previousScene) {
                if (m_isAutoDelete && m_previousScene) {
                    DeleteGO(m_previousScene);
                    m_previousScene = nullptr;
                }
            }
            DeleteGO(this);
        }
        break;
    }

    // ★値を渡す
    m_diamondWipe.SetProgress(m_transitionVal);
    m_loadingCircle.Update();
    m_text.Update();

    // --- ★ドットのアニメーションロジックの追加 ---

    float dotSpeed = 0.5f; // 0.5秒ごとにドットが切り替わる

    // 現在の秒数を dotSpeed で割って、floorで切り捨てる
    // 0.0s～0.5s: 0, 0.5s～1.0s: 1, 1.0s～1.5s: 2, 1.5s～2.0s: 3 (次へ)
    int step = static_cast<int>(g_gameTime->GetTotalTime() / dotSpeed);

    // 4ステップでループ (0, 1, 2, 3 -> 0に戻る)
    // 0: '.', 1: '..', 2: '...', 3: '' (全部非表示) の4パターン
    int pattern = step % 4;

    // ドットの表示/非表示を切り替える
    // SetActive(true) のドットだけが Draw されます。

    // 0番目のドットは、パターンが 0, 1, 2 のとき表示
    m_dots[0].SetActive(pattern != 3);

    // 1番目のドットは、パターンが 1, 2 のとき表示
    m_dots[1].SetActive(pattern >= 1 && pattern <= 2);

    // 2番目のドットは、パターンが 2 のときのみ表示
    m_dots[2].SetActive(pattern == 2);

    // --- 忘れずに Update を呼ぶ ---
    m_text.Update();
    for (int i = 0; i < 3; i++) {
        m_dots[i].Update();
    }
}

void Loading::Render(RenderContext& rc)
{
    if (m_state == State::FadeIn && m_previousScene) {
        m_previousScene->Render(rc);
    }

    m_diamondWipe.Draw(rc);

    if (m_transitionVal > 0.0f) {
        m_loadingCircle.Draw(rc);
        //m_text.Draw(rc);
        // ★ドットの描画を追加
        for (int i = 0; i < 3; i++) {
            m_dots[i].Draw(rc);
        }
    }

}