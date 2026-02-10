#include "stdafx.h"
#include "TitleState.h"
#include "InGameNomalState.h"
#include "Game.h"

void TitleState::Initialize(Game* game) {
    m_game = game;
    m_titleSprite.Init("Assets/Karimodel/TitleFullSt.DDS", 1920, 1080);
}

void TitleState::Update(Game* game) {
    // Aボタンでゲーム本編へ
    if (g_pad[0]->IsTrigger(enButtonA)) {
        m_game->ChangeState(new InGameNomalState());
    }
	m_titleSprite.Update();
}

void TitleState::Render(RenderContext& rc) {
    // ここで背景や文字を描画（今は空でも真っ暗なタイトルになります）
	m_titleSprite.Draw(rc);
}