#include "stdafx.h"
#include "InGameRythmState.h"
#include "Game.h"

//リズムゲームのステート
//順番：カットイン→五線譜拡大→リズムゲーム→終了→通常のゲームに戻る
//「私の歌があなたを導く…！」

void InGameRythmState::Initialize(Game* game)
{
    m_game = game;
    m_phase = RythmPhase::CutIn;
    m_timer = 0.0f;

}

void InGameRythmState::Update(Game* game)
{
    m_timer += g_gameTime->GetFrameDeltaTime(); // デルタタイム取得関数

    switch (m_phase)
    {
    case RythmPhase::CutIn:
        UpdateCutIn();
        break;
    case RythmPhase::StaffZoom:
        UpdateStaffZoom();
        break;
    case RythmPhase::Gameplay:
        UpdateGameplay();
        break;
    case RythmPhase::End:
        FinishRythm();
        break;
    }
}

void InGameRythmState::Render(RenderContext& rc)
{
    // フェーズごとの描画
    switch (m_phase)
    {
    case RythmPhase::CutIn:
        DrawCutIn(rc);
        break;
    case RythmPhase::StaffZoom:
        DrawStaffZoom(rc);
        break;
    case RythmPhase::Gameplay:
        DrawGameplay(rc);
        break;
    case RythmPhase::End:
        // 何も描画しないかフェード
        break;
    }
}



void InGameRythmState::UpdateCutIn()
{
    if (m_timer > 1.5f) // カットイン1.5秒
    {
        m_phase = RythmPhase::StaffZoom;
        m_timer = 0.0f;
    }
}

void InGameRythmState::UpdateStaffZoom()
{
    if (m_timer > 0.8f) // 五線譜拡大0.8秒
    {
        m_phase = RythmPhase::Gameplay;
        m_timer = 0.0f;
    }
}

void InGameRythmState::UpdateGameplay()
{
    // TODO: ノーツ判定・判定UI更新
    // 仮: タイムアップで終了
    if (/* ゲーム終了判定 */ false)
    {
        m_isClear = true/* 判定結果 */;
        m_phase = RythmPhase::End;
    }
}

void InGameRythmState::FinishRythm()
{
    // InGameNormalStateに戻す or ResultStateへ
    m_game->PopState(); // リズムゲーム終了後、通常のゲームに戻る
    
 
}

void InGameRythmState::DrawCutIn(RenderContext& rc)
{
    // カットインスプライト描画
}

void InGameRythmState::DrawStaffZoom(RenderContext& rc)
{
    // 五線譜拡大アニメ描画
}

void InGameRythmState::DrawGameplay(RenderContext& rc)
{
    // ノーツ・判定UI描画
}