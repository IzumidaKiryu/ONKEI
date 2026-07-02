#pragma once


class StartUI :public IGameObject
{
public:
	StartUI();
	~StartUI();
	void Init();
	void Update();
	void SetCount(int count){ m_currentCount = count; }; // カウントダウン処理を行う関数
	void Render(RenderContext& rc);

	SpriteRender m_backSprite;//赤帯のUI
	SpriteRender m_startText;//スタートテキストのUI
	FontRender m_countText;//カウントダウンのテキストUI

	int m_currentCount = 3; // カウントダウンの現在の数値
	float m_StartUITimer = 0.0f; // 赤帯のアニメーション用タイマー
};

