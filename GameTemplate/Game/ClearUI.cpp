#include "stdafx.h"
#include "ClearUI.h"


namespace {
	// 赤帯のUIの定数設定
	const Vector3 BACK_POSITION = Vector3(0.0f, 0.0f, 0.0f);//赤帯の座標
	const Vector3 BACK_SCALE = Vector3(1.0f, 1.0f, 1.0f);//赤帯の大きさ
	const Vector4 BACK_COLOR = Vector4(1.0f, 0.0f, 0.0f, 0.5f); //赤帯の色設定
	//クリアテキストのUIの定数設定
	const Vector3 CLEAR_TEXT_POSITION = Vector3(0.0f, 0.0f, 0.0f);//クリアテキストの座標
	const Vector3 CLEAR_TEXT_SCALE = Vector3(1.0f, 1.0f, 1.0f);//クリアテキストの大きさ
}

ClearUI::ClearUI()
{

}

ClearUI::~ClearUI()
{

}

void ClearUI::Init()
{
	//赤帯の初期化
	m_backSprite.Init("Assets/sprite/white.DDS", 1920.0f, 250.0f);
	m_backSprite.SetPosition(BACK_POSITION);
	m_backSprite.SetScale(BACK_SCALE);
	m_backSprite.SetMulColor(BACK_COLOR);
	//クリアテキストの初期化
	m_clearText.Init("Assets/UI/gameClearFont.DDS", 1920.0f, 1080.0f);
	m_clearText.SetPosition(CLEAR_TEXT_POSITION);
	m_clearText.SetScale(CLEAR_TEXT_SCALE);

	//赤帯のアニメーション用タイマーを初期化
	m_clearUITimer = 0.0f;

	//更新
	m_backSprite.Update();
	m_clearText.Update();
}

void ClearUI::Update()
{
	

	// 赤帯のアニメーション用タイマーを更新
	m_clearUITimer += g_gameTime->GetFrameDeltaTime();

	//更新
	m_backSprite.Update();
	//タイマーが2秒以上経過していたらクリアテキストを更新
	if (m_clearUITimer >= 1.0f) {
		m_clearText.Update();
	}

	//タイマーが5秒以上経過していたら自身を削除させる
	//if (m_clearUITimer >= 5.0f) {
	//	// ここで自身を削除させる
	//	DeleteGO(this);
	//}

}

void ClearUI::Render(RenderContext & rc)
{
	m_backSprite.Draw(rc);
	//タイマーが2秒以上経過していたらクリアテキストを描画
	if (m_clearUITimer >= 1.0f) {
		m_clearText.Draw(rc);
	}
	
}
