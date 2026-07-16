#include "stdafx.h"
#include "TitleState.h"
#include "InGameNomalState.h"
#include "Game.h"
#include "sound/SoundEngine.h"

namespace {
    //文字の表示。
    const Vector3 GAME_FONT_POSITION = { -200.0f,-250.0f,0.0f };
}

void TitleState::Initialize(Game* game) {
    m_game = game;
    m_titleSprite.Init("Assets/UI/title.DDS", 1920, 1080);

    //文字の表示
    m_titleFont.SetText(L"Press A Button");
    m_titleFont.SetPosition(GAME_FONT_POSITION);
	m_titleFont.SetColor(m_fontColor);

}

void TitleState::Update(Game* game) {
    // Aボタンでゲーム本編へ
    if (g_pad[0]->IsTrigger(enButtonA)&&m_isChangeFlag==false) {
		//タイトルBGMの再生。
		g_soundEngine->ResistWaveFileBank(5,"Assets/sound/buttonTap.wav");
		m_tapSound = NewGO<SoundSource>(5);
		m_tapSound->Init(5);
		m_tapSound->Play(false);
		//ノーマルステートに遷移する。
        m_game->ChangeState(new InGameNomalState());
		m_isChangeFlag = true;
    }
	//タイマーを加算。
	m_timer += g_gameTime->GetFrameDeltaTime();
	m_titleSprite.Update();
	FontFade();
}

void TitleState::FontFade()
{
	if (!m_isFontFade)
	{
		//フォントのフェードインの処理。
		m_fontColor.r += 0.01f * m_timer / m_maxTitleTime;
		m_fontColor.g += 0.01f * m_timer / m_maxTitleTime;
		m_fontColor.b += 0.01f * m_timer / m_maxTitleTime;
		m_fontColor.a += 0.01f * m_timer / m_maxTitleTime;

		m_titleFont.SetColor(m_fontColor);
		
		if (m_fontColor.a >= 1.0f)
		{
			m_isFontFade = true;
			m_fontColor.r = 1.0f;
			m_fontColor.g = 1.0f;
			m_fontColor.b = 1.0f;
			m_fontColor.a = 1.0f;
			
			m_titleFont.SetColor(m_fontColor);
			m_timer = 0.0f;
			
		}
	}
	else if(m_isFontFade)
	{
		//フォントのフェードアウトの処理。
		m_fontColor.r -= 0.01f * m_timer / m_maxTitleTime;
		m_fontColor.g -= 0.01f * m_timer / m_maxTitleTime;
		m_fontColor.b -= 0.01f * m_timer / m_maxTitleTime;
		m_fontColor.a -= 0.01f * m_timer / m_maxTitleTime;
		
		m_titleFont.SetColor(m_fontColor);
		
		if (m_fontColor.a <= 0.0f)
		{
			m_isFontFade = false;
			m_fontColor.r = 0.0f;
			m_fontColor.g = 0.0f;
			m_fontColor.b = 0.0f;
			m_fontColor.a = 0.0f;
			
			m_titleFont.SetColor(m_fontColor);
			m_timer = 0.0f;
		}
	}
    m_titleFont.SetColor(m_fontColor);
}

void TitleState::Render(RenderContext& rc) {
    // ここで背景や文字を描画（今は空でも真っ暗なタイトルになります）
	m_titleSprite.Draw(rc);
	//文字の描画
	m_titleFont.Draw(rc);
}