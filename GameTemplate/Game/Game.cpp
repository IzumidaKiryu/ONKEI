#include "stdafx.h"
#include "Game.h"
#include "TitleState.h"
#include "Loading.h"

// デストラクタ: ゲーム終了時にメモリを掃除する
Game::~Game() {
	//ヒットストップ中にゲームが終わっても時間が止まったままにならないよう戻しておく。
	g_gameTime->DisableFixedFrameDeltaTime();
}

// ゲーム開始時の初期化
bool Game::Start() {
    // 最初のシーン（タイトル画面）を生成してセット
    // unique_ptrではなく、生のポインタを new で渡す設計に変更
    FarstState(new TitleState());
    return true;
}

// 毎フレームの更新処理
void Game::Update() {
    //ヒットストップの解除処理。ステートより先に処理する。
    UpdateHitStop();

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

void Game::FarstState(IGameState* firstState)
{
    // 最初のシーンをセットする関数
    // ここではローディングシーンを挟まずに直接切り替える
    if (m_currentState) {
        delete m_currentState;
        m_currentState = nullptr;
    }
    m_currentState = firstState;
    if (m_currentState) {
        m_currentState->Initialize(this);
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
        // 1. 今のステートを止める（例：Updateをスキップするフラグを立てるなど）
        m_currentState->OnPause();
        m_stateStack.push(m_currentState);
    }
    m_currentState = overlayState;
    if (m_currentState) {
        m_currentState->Initialize(this);
    }
}

// 重ねたシーンを破棄して、前のシーンに戻る
//ヒットストップの要求。撃破の瞬間にゲーム全体の時間を止めて手応えを出す。
//同じフレームに複数の敵が死んでも、長い方が優先される。
void Game::RequestHitStop(int frames)
{
	if (frames > m_hitStopFrames) {
		m_hitStopFrames = frames;
	}
	//デルタタイムを0に固定して、ゲーム全体の時間を止める。
	g_gameTime->EnableFixedFrameDeltaTime(0.0f);
}

//ヒットストップの解除。
//止めている間はデルタタイムが0なので、残りを「時間」で数えると永久に減らず固まる。
//そのため必ず「フレーム数」で数える。
void Game::UpdateHitStop()
{
	if (m_hitStopFrames <= 0) {
		return;
	}
	m_hitStopFrames--;
	if (m_hitStopFrames <= 0) {
		m_hitStopFrames = 0;
		g_gameTime->DisableFixedFrameDeltaTime();
	}
}

void Game::PopState() {
    if (m_currentState) {
        delete m_currentState;
        m_currentState = nullptr;
    }

    if (!m_stateStack.empty()) {
        m_currentState = m_stateStack.top();
        m_stateStack.pop();
        // 2. 戻ってきたステートを再開させる
        m_currentState->OnResume();
    }
}