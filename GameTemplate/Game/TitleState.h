#pragma once
#include "GameState.h"

class TitleState : public IGameState {
public:
    void Initialize(Game* game) override;
    void Update(Game* game) override;
	void FontFade();//文字のフェードイン・アウトを制御する関数
    void Render(RenderContext& rc) override;
private:
	SpriteRender m_titleSprite;
	FontRender m_titleFont; //Prees a buttonの文字を描画するためのFontRender
	Vector4 m_fontColor = {0.0f, 0.0f, 0.0f, 1.0f}; //文字の色
	Game* m_game;

	float m_timer = 0.0f; //タイマー
	float m_maxTitleTime = 1.0f; //タイトルの最大時間

	bool m_isFontFade = true; //文字のフェードイン・アウトの切り替えフラグ
	bool m_isChangeFlag = false;//ステートの変更が行われたかどうか（1回のみに限定するため）
};