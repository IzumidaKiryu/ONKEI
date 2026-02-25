#include "stdafx.h"
#include "Game.h"
#include "TitleState.h"
#include "Loading.h"

// デストラクタ: ゲーム終了時にメモリを掃除する
Game::~Game() {
  
}

// ゲーム開始時の初期化
bool Game::Start() {
    // 最初のシーン（タイトル画面）を生成してセット
    // unique_ptrではなく、生のポインタを new で渡す設計に変更
    ChangeState(new TitleState());
    return true;
}

// 毎フレームの更新処理
void Game::Update() {
    // 現在のステートに更新処理を丸投げする
    if (m_currentState) {
        m_currentState->Update(this);
    }
}

// 毎フレームの描画処理
void Game::Render(RenderContext& rc) {
    // 現在のステートに描画処理を丸投げする
    if (m_currentState) {
        m_currentState->Render(rc);
    }
}

// シーンを完全に切り替える（古いシーンは削除）
void Game::ChangeState(IGameState* nextState) {
    auto* loading = NewGO<Loading>(1, "loading");
	// ★ローディングシーンを挟む
    loading->Init(
        // --- 第1引数: 次のシーンを作る関数 ---
        [=]() {
            // 1. 古いステートのメモリを解放
        if (m_currentState) {
            delete m_currentState;
            m_currentState = nullptr;
        }

        // 2. 新しいステートを現在のステートとして保持
        m_currentState = nextState;

        // 3. 新しいステートを初期化
        if (m_currentState) {
            m_currentState->Initialize(this);
        }
     },

        // --- ★第2引数: 準備完了チェック関数
        nullptr

   
    );

   
}

// 現在のシーンを保持したまま、新しいシーンを上に重ねる（例：一時停止メニュー）
void Game::PushState(IGameState* overlayState) {
    if (m_currentState) {
        m_stateStack.push(m_currentState);
    }
    m_currentState = overlayState;
    if (m_currentState) {
        m_currentState->Initialize(this);
    }
}

// 重ねたシーンを破棄して、前のシーンに戻る
void Game::PopState() {
    if (m_currentState) {
        delete m_currentState;
        m_currentState = nullptr;
    }

    if (!m_stateStack.empty()) {
        m_currentState = m_stateStack.top();
        m_stateStack.pop();
    }
}