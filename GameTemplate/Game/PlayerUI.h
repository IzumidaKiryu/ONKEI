#pragma once
#include "UIGaugeArc.h"

class Player;
class PlayerUI : public IGameObject
{
public:
	PlayerUI();
	~PlayerUI();
	bool Start() override;
	void Update() override;
	//被ダメ時キャラアイコンを赤く点滅させるための関数
	//void FlashCharaIcon(float flashAmount) {
	//	// flashAmountは0.0fから1.0fの範囲で、1.0fが完全に赤く点滅している状態を表すと仮定
	//	Vector4 flashColor = { 1.0f, 1.0f - flashAmount, 1.0f - flashAmount, 1.0f }; // 赤く点滅させるための色
	//	m_charaIcon.SetMulColor(flashColor);
	//}
	void Render(RenderContext& rc) override;

	float m_hpBarWidth = 300.0f; // HPバーの幅
	float m_hpBarHeight = 50.0f; // HPバーの高さ	
	float m_currentHP = 300.0f; // 現在のHP
	float m_maxHP = 300.0f; // 最大HP
	float m_attackGaugeRate = 1.0f; //100%で1.0f、0%で0.0fになる攻撃ゲージの割合
	float m_flashTimer = 0.0f; // 点滅のタイマー
private:
	SpriteRender m_hpBarBack;  // ゲージ背景
	SpriteRender m_hpBarFront; // ゲージ本体
	SpriteRender m_charaIcon; // キャラアイコン
	SpriteRender m_buttonA;//Aボタン
	SpriteRender m_buttonB;//Bボタン
	nsK2Engine::UIGaugeArc* m_AttackGauge=nullptr; // 攻撃ゲージ
	Player* m_player = nullptr;
};

