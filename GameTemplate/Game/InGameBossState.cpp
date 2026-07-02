#include "stdafx.h"
#include "InGameBossState.h"
#include "Game.h"
#include "Player.h"
#include "GameCamera.h"
#include "EnemyManager.h"
#include "PlayerUI.h"
#include "StartUI.h"
#include "ClearUI.h"
#include "ResultState.h"
#include "InGameNomalState.h"
#include "InGameRythmState.h"
#include "Stage.h"
#include "SkillButton.h"

//boss戦のインゲームステート
namespace {
	const float SKILLGAUGEMAX = 100.0f; // スキルゲージの最大値

}

InGameBossState::~InGameBossState()
{
	DeleteGO(m_player);
	DeleteGO(m_camera);
	DeleteGO(m_enemyManager);
	DeleteGO(m_playerUI);
	DeleteGO(m_clearUI);
	DeleteGO(stage);
	DeleteGO(m_skillButton);
	DeleteGO(m_skyCube);
}

void InGameBossState::Initialize(Game* game)
{
	m_game = game;
	m_player = NewGO<Player>(0, "player");
	// 「あなたの親は私（NormalState）ですよ」と教える
	m_player->SetParentState(this);

	m_enemyManager = NewGO<EnemyManager>(0, "EnemyManager");
	m_enemyManager->BossSpawn();

	m_camera = NewGO<GameCamera>(0, "camera");
	m_playerUI = NewGO<PlayerUI>(0, "playerUI");
	g_camera3D->SetPosition(Vector3(-700.0f, 500.0f, 100.0f));
	// Stageを生成（NewGOはエンジンのリストに登録する関数）
	stage = NewGO<Stage>(0, "Stage");

	// 自分を親としてセットする
	stage->SetParentState(this);

	m_skillButton = NewGO<SkillButton>(0, "skillButton");
	m_skillButton->Init("Assets/UI/skill.DDS", SKILLGAUGEMAX); // アイコンとクールタイムを指定して初期化

	//空の作成。
	m_skyCube = NewGO<SkyCube>(0, "skyCube");
	m_skyCube->SetType(enSkyCubeType_Night);
	m_skyCube->SetScale(1000.0f);

	//空の光の強さ。
	//m_skyCube->SetLuminance(m_skyLuminance);

	//空の光から影響する環境光の強さ
	//g_renderingEngine->SetAmbientByIBLTexture(m_skyCube->GetTextureFilePath(), m_skyAmbient);

}

void InGameBossState::Update(Game* game)
{
	//ゲーム開始UIの生成
	StartFontFade();

	//ゲーム開始のフラグが立ったら
	if (m_isGameStart) {

		StageClear();

		//ゲームオーバー、ゲームクリアのフラグが立っているかを確認して、立っていたらそれぞれのステートに遷移する
		if (m_isGameOver == true) {
			m_enemyManager->ClearAllEnemies();
			m_game->m_scoreTimer = m_game->m_gameTimer;//プレイ時間を記録
			m_game->ChangeState(new ResultState(ResultState::ResultType::enGameOver));
			m_isGameOver = false; // フラグをリセットしておく（次のプレイでゲームオーバーになったときに正しく遷移するように）
			return;
		}
		else if (m_isGameClear == true) {
			m_enemyManager->ClearAllEnemies();

			// 1. まだ生成されていなければ生成する
			if (!m_isClearUI) {
				m_clearUI = NewGO<ClearUI>(0, "clearUI");
				m_clearUI->Init();
				m_game->m_scoreTimer = m_game->m_gameTimer;//プレイ時間を記録
				m_isClearUI = true;
			}
			// 2. 生成済みなら、生成処理とは別にタイマーをチェックする
			else {
				// m_clearUI はすでに存在するので安全
				if (m_clearUI->m_clearUITimer >= 3.0f) {
					m_game->ChangeState(new ResultState(ResultState::ResultType::enClear));
					m_isGameClear = false;
					return; // 遷移したら即終了
				}
			}
		}
		//判定が何もないなら
		else {
			//スコアタイマーを加算
			m_game->m_gameTimer += g_gameTime->GetFrameDeltaTime();
		}

		//スキルゲージが100溜まったら
		if (m_player->m_playerSkillGauge >= SKILLGAUGEMAX) {
			m_isSPButtonIsReady = true; // SPボタンが押せる状態にする
		}
		else {
			m_isSPButtonIsReady = false; // SPボタンが押せない状態にする
		}
		//SPボタンが押せる状態でLB1が押されたらSPボタンを発動させる
		if (m_isSPButtonIsReady && g_pad[0]->IsTrigger(enButtonB)) {//ボタンは変える！
			//SPボタンの発動処理をここに書く
			m_skillButton->UseSkill();//まず、スキルボタンクラス側の発動関数を呼ぶ。
			m_player->m_playerSkillGauge = 0;//プレイヤー側のスキルゲージを0にする
			m_game->PushState(new InGameRythmState());//リズムゲームのステートをPushする。これでリズムゲームのステートが表に出る。
		}

		m_skillButton->Update((float)m_player->m_playerSkillGauge);//スキルボタンにはプレイヤーのスキルゲージを渡す。
	}

	
}

void InGameBossState::StartFontFade()
{
	if (m_isGameStart) return;

	if (!m_isStartUI) {
		m_startUI = NewGO<StartUI>(0, "startUI");
		m_startUI->Init();
		m_isStartUI = true;
		m_startUITimer = 3.0f;
		m_startUI->m_currentCount = 3; // ★State側でカウントを保持しておく
		m_startUI->SetCount(m_startUI->m_currentCount); // 初期値をセット
		return; // ★生成したフレームはここで抜ける（Initした直後のフレームで書き換えない）
	}


	m_startUITimer -= g_gameTime->GetFrameDeltaTime();
	int nextCount = (int)ceil(m_startUITimer);

	// 数値が変化した時だけ SetCount を呼ぶ
	if (nextCount != m_startUI->m_currentCount) {
		m_startUI->m_currentCount = nextCount;
		if (m_startUI) m_startUI->SetCount(m_startUI->m_currentCount);
	}

	if (m_startUITimer <= -0.5f) {
		m_isGameStart = true;
		DeleteGO(m_startUI);
		m_startUI = nullptr; // ★削除したら必ずポインタをnullにする！
	}

}

void InGameBossState::Render(RenderContext& rc)
{
	m_skillButton->Draw(rc);

}

void InGameBossState::OnPause()
{
	m_camera->Deactivate();
	m_enemyManager->Deactivate();
	m_playerUI->Deactivate();
	this->Deactivate(); // ステートを非アクティブにする
}

void InGameBossState::OnResume()
{
	//自分がNewGOしたクラスたちもDeActiveteされているはずなので、必要に応じてそれらもActivateする
	m_camera->Activate();
	m_enemyManager->Activate();
	m_playerUI->Activate();
	this->Activate(); // ステートをアクティブにする
}

void InGameBossState::StageClear()
{
	if ( m_enemyManager->GetBossDeathFlag()==true && m_isChangeIsReady == false) {
		m_isGameClear = true;
		m_isChangeIsReady = true; // ステート遷移が発動したことを示すフラグを立てる
	}

}

void InGameBossState::GameOver()
{
	//プレイヤーのHPが0になったらゲームオーバーにする。
	if (m_player->m_playerHP <= 0 && m_isChangeIsReady == false) {
		//ゲームオーバーの処理を行う。
		m_isGameOver = true;
		m_isChangeIsReady = true; // ステート遷移が発動したことを示すフラグを立てる
	}
}
