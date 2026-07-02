#include "stdafx.h"
#include "StartUI.h"


namespace 
{
	//赤帯のUIの定数設定
	const Vector3 BACK_POSITION = Vector3(0.0f, 0.0f, 0.0f);//赤帯の座標
	const Vector3 BACK_SCALE = Vector3(1.0f, 1.0f, 1.0f);//赤帯の大きさ
	const Vector4 BACK_COLOR = Vector4(1.0f, 0.0f, 0.0f, 0.5f); //赤帯の色設定
	//スタートテキストのUIの定数設定
	const Vector3 START_TEXT_POSITION = Vector3(0.0f, 0.0f, 0.0f);//スタートテキストの座標
	const Vector3 START_TEXT_SCALE = Vector3(1.0f, 1.0f, 1.0f);//スタートテキストの大きさ
	//カウントダウンテキストのUIの定数設定
	const Vector3 COUNT_TEXT_POSITION = Vector3(-80.0f, 50.0f, 0.0f);//カウントダウンテキストの座標
	const float COUNT_TEXT_SCALE = 3.0f;//カウントダウンテキストの大きさ
	const Vector4 COUNT_TEXT_COLOR = Vector4(1.0f, 1.0f, 1.0f, 1.0f); //カウントダウンテキストの色設定
}

StartUI::StartUI()
{

}

StartUI::~StartUI()
{

}

void StartUI::Init()
{
	//赤帯の初期化
	m_backSprite.Init("Assets/sprite/white.DDS", 1920.0f, 250.0f);
	m_backSprite.SetPosition(BACK_POSITION);
	m_backSprite.SetScale(BACK_SCALE);
	m_backSprite.SetMulColor(BACK_COLOR);
	//スタートテキストの初期化
	m_startText.Init("Assets/UI/gameStartFont.DDS", 1920.0f, 1080.0f);
	m_startText.SetPosition(START_TEXT_POSITION);
	m_startText.SetScale(START_TEXT_SCALE);

	std::wstring countStr = std::to_wstring(m_currentCount);
	m_countText.SetText(countStr.c_str()); // 最初の一回はここでテキストをセット

	// NomalTaskUI に倣ってポジションや色を確実にセット
	m_countText.SetPosition(COUNT_TEXT_POSITION);
	m_countText.SetScale(COUNT_TEXT_SCALE);
	m_countText.SetColor(COUNT_TEXT_COLOR);

	//UIの更新
	m_backSprite.Update();
	m_startText.Update();

	//赤帯のアニメーション用タイマーを初期化
	m_StartUITimer = 0.0f;
}

void StartUI::Update()
{
	// 赤帯のアニメーション用タイマーを更新
	m_StartUITimer += g_gameTime->GetFrameDeltaTime();

	// ★ここが重要：カウントが変化するたびにテキストを書き換える
	// InGameNomalStateでセットされた m_currentCount を文字列に変換して適用
	std::wstring countStr = std::to_wstring(m_currentCount);
	m_countText.SetText(countStr.c_str());

	// UIの更新
	m_backSprite.Update();
	m_startText.Update();
}

void StartUI::Render(RenderContext& rc)
{
	m_backSprite.Draw(rc);

	// 3秒経過する前（カウントダウン中）は数字を描画
	if (m_StartUITimer < 3.0f) {
		m_countText.Draw(rc);
	}
	// 3秒経過したら「START!」を描画
	else {
		m_startText.Draw(rc);
	}
	
}