#pragma once
class ClearUI:public IGameObject
{
public:
	ClearUI();
	~ClearUI();
	void Init();
	void Update();
	void Render(RenderContext& rc);

	float m_clearUITimer = 0.0f; // 赤帯のアニメーション用タイマー

private:
	SpriteRender m_backSprite; // 赤帯のUI
	SpriteRender m_clearText; // クリアテキストのUI

	
};

