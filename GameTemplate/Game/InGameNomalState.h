#pragma once
#include "GameState.h"
#include "sound/SoundSource.h"


//雑魚敵戦のインゲームステート
class Player;
class GameCamera;
class EnemyManager;
class PlayerUI;
class Stage;
class SkillButton;
class TaskBarUI;
class StartUI;
class ClearUI;
class ResultState;

class InGameNomalState : public IGameState {

public:
	~InGameNomalState();
    void Initialize(Game* game) override;
    void Update(Game* game) override;
    void Render(RenderContext& rc) override;
	void StartFontFade(); // フォントのフェードイン・アウト処理を行う関数
	void SpawnEnemy(); // 敵をスポーンさせる関数
	void CreateUI(); // UIを作成する関数
	void OnPause() override; // ポーズ状態に入るときの処理
	void OnResume() override; // ポーズ状態から復帰するときの処理
	void ResetFlags();
	//クラス固有の関数
	void UpdateSPButton();//SPボタンの状態を更新する関数。SPボタンが押せる状態かどうかを更新する。(割合計算もこの関数内で行い描画の差異を出す。)
	void StageClear();//ステージクリア処理。ゲームクリアフラグを立てる。
	void UpdateTimeLimit();//残り時間を減算し、残り30秒でラッシュに切り替える。
	void UpdateScore();//撃破スコアとリズムゲームスコアを合計してGameに反映する。
	void EndGameSpawn();//ゲーム終了時に敵のスポーンを打ち切って一掃する。
	void GameOver();//ゲームオーバー処理。ゲームオーバーフラグを立てる。

	//残りこの秒数を切ったらラッシュ開始（敵の最大数と湧く速さが倍になる）
	static constexpr float RUSH_START_REMAIN_TIME = 30.0f;

private:
	Game* m_game;//切り替えに使用(もっといい方法ある気がする...)
	//クラス固有の変数

	Player* m_player;
	PlayerUI* m_playerUI;
   
	GameCamera* m_camera;
	EnemyManager* m_enemyManager;
	Stage* m_stage;
	SkillButton* m_skillButton;
	SkyCube* m_skyCube;
	ResultState* m_resultState;

	TaskBarUI* m_taskBarUI;
	StartUI* m_startUI;
	ClearUI* m_clearUI;

	
	float m_currentSP = 0.0f; // 現在の信仰値
	float m_maxSP = 100.0f; // 最大信仰値
	float m_skyLuminance = 0.01f;//空の明るさ。
	float m_skyAmbient = 0.01f;	//空の明るさの影響を受ける環境光。
	float m_StartUITimer = 0.0f; // StartUIの表示時間を管理するタイマー

	bool m_isGameUI = false; // ゲームUIが作成されたかどうかのフラグ
	bool m_isStartUI = false; // StartUIが作成されたかどうかのフラグ
	bool m_isClearUI = false;//CrearUIが生成されたかどうかのフラグ
	bool m_isEnemySpawned = false; // 敵がスポーンされたかどうかのフラグ
	bool m_isGameStart = false; // ゲーム開始フラグ
	bool m_isGameOver = false; // ゲームオーバーフラグ
	bool m_isGameClear = false; // ゲームクリアフラグ
	bool m_isSPButtonIsReady = false; // SPボタンが押セル状態かどうかのフラグ（trueの時は押せる）。
	bool m_isChangeIsReady = false; // ステート遷移が発動したかどうかのフラグ（trueの時は遷移が発動している）。遷移が発動している間はUpdate内で遷移処理を行わないようにするためのフラグ。
	bool m_isRushStarted = false; // ラッシュに切り替え済みかどうかのフラグ
};