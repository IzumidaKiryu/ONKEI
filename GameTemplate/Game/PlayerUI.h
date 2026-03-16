#pragma once
class Player;
class PlayerUI : public IGameObject
{
public:
	PlayerUI();
	~PlayerUI();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

	float m_hpBarWidth = 200.0f; // HPバーの幅
	float m_hpBarHeight = 50.0f; // HPバーの高さ	
	float m_currentHP = 300.0f; // 現在のHP
	float m_maxHP = 300.0f; // 最大HP
private:
	SpriteRender m_hpBarBack;  // ゲージ背景
	SpriteRender m_hpBarFront; // ゲージ本体
	Player* m_player = nullptr;
};

