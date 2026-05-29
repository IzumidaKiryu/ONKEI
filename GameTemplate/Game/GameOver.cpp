#include "stdafx.h"
#include "GameOver.h"
#include "TitleState.h"

GameOver::GameOver()
{

}

GameOver::~GameOver()
{

}

void GameOver::Init()
{
	m_sprite.Init("Assets/UI/gameover.DDS", 1920, 1080);
	m_sprite.Update();
}

void GameOver::Update()
{
	// Aボタンでタイトルへ
	if (g_pad[0]->IsTrigger(enButtonA)) {
		m_titleState = NewGO<TitleState>(0, "titleState");
		DeleteGO(this);
	}
}

void GameOver::Render(RenderContext& rc)
{
	m_sprite.Draw(rc);
}