#pragma once
#include "GameState.h"


//雑魚敵戦のインゲームステート
class Player;
class GameCamera;
class EnemyManager;
class PlayerUI;
class Stage;
class SkillButton;
class InGameNomalState : public IGameState {
public:
	~InGameNomalState();
    void Initialize(Game* game) override;
    void Update(Game* game) override;
    void Render(RenderContext& rc) override;
	void OnPause() override; // ポーズ状態に入るときの処理
	void OnResume() override; // ポーズ状態から復帰するときの処理
	//クラス固有の関数
	void UpdateSPButton();//SPボタンの状態を更新する関数。SPボタンが押せる状態かどうかを更新する。(割合計算もこの関数内で行い描画の差異を出す。)
	void StageClear();//ステージクリア処理。ゲームクリアフラグを立てる。

private:
	Game* m_game;//切り替えに使用(もっといい方法ある気がする...)
	//クラス固有の変数

	Player* m_player;
	PlayerUI* m_playerUI;
   
	GameCamera* m_camera;
	EnemyManager* m_enemyManager;
	Stage* stage;
	SkillButton* m_skillButton;

	float m_currentSP = 0.0f; // 現在の信仰値
	float m_maxSP = 100.0f; // 最大信仰値

	bool m_isGameOver = false; // ゲームオーバーフラグ
	bool m_isGameClear = false; // ゲームクリアフラグ
	bool m_isSPButtonIsReady = false; // SPボタンが押セル状態かどうかのフラグ（trueの時は押せる）。
	bool m_isChangeIsReady = false; // ステート遷移が発動したかどうかのフラグ（trueの時は遷移が発動している）。遷移が発動している間はUpdate内で遷移処理を行わないようにするためのフラグ。
};