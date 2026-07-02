#pragma once
#include "GameState.h"


//boss戦のインゲームステート
class InGameNomalState;
class Player;
class GameCamera;
class EnemyManager;
class PlayerUI;
class StartUI;
class ClearUI;
class Stage;
class SkillButton;

class InGameBossState : public IGameState {
public:
    ~InGameBossState();
    void Initialize(Game* game) override;
    void Update(Game* game) override;
    void Render(RenderContext& rc) override;
	void OnPause() override; // ポーズ状態に入るときの処理
	void OnResume() override; // ポーズ状態から復帰するときの処理
	
    //クラス固有の関数
	void StartFontFade(); // フォントのフェードイン・アウト処理を行う関数
    void StageClear();//ステージクリア処理。ゲームクリアフラグを立てる。
	void GameOver();

private:
    Game* m_game;
	//クラス固有の変数
	InGameNomalState* m_inNomalState;
	Player* m_player;
	PlayerUI* m_playerUI;
	StartUI* m_startUI;
	ClearUI* m_clearUI;

	GameCamera* m_camera;
	EnemyManager* m_enemyManager;
	Stage* stage;
	SkillButton* m_skillButton;
	SkyCube* m_skyCube;

	float m_currentSP = 0.0f; // 現在の信仰値
	float m_maxSP = 100.0f; // 最大信仰値

	float m_skyLuminance = 0.01f;//空の明るさ。
	float m_skyAmbient = 0.01f;	//空の明るさの影響を受ける環境光。

	float m_startUITimer = 0.0f;

	bool m_isStartUI = false; // StartUIが作成されたかどうかのフラグ
	bool m_isClearUI = false;//CrearUIが生成されたかどうかのフラグ
	bool m_isGameStart = false;//ゲーム開始したかのフラグ
	bool m_isGameOver = false; // ゲームオーバーフラグ
	bool m_isGameClear = false; // ゲームクリアフラグ
	bool m_isSPButtonIsReady = false; // SPボタンが押セル状態かどうかのフラグ（trueの時は押せる）。
	bool m_isChangeIsReady = false; // ステート遷移が発動したかどうかのフラグ（trueの時は遷移が発動している）。遷移が発動している間はUpdate内で遷移処理を行わないようにするためのフラグ。
};