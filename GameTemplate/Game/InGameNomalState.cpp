#include "stdafx.h"
#include "InGameNomalState.h"
#include "Game.h"
#include "Player.h"
#include "GameCamera.h"
#include "EnemyManager.h"
#include "PlayerUI.h"
#include "ResultState.h"
#include "InGameBossState.h"
#include "InGameRythmState.h"
#include "Stage.h"
#include "SkillButton.h"
namespace
{
	int CREACOUNT=11; // 軟体倒したらクリアにするか変数
	const float SKILLGAUGEMAX = 100.0f; // スキルゲージの最大値
}

InGameNomalState::~InGameNomalState() {
	DeleteGO(m_player);
	DeleteGO(m_camera);
	DeleteGO(m_enemyManager);
	DeleteGO(m_playerUI);
	DeleteGO(stage);
}

void InGameNomalState::Initialize(Game* game) {
    m_game = game;
	m_player = NewGO<Player>(0,"player");
	// 「あなたの親は私（NormalState）ですよ」と教える
	m_player->SetParentState(this);

	m_enemyManager=NewGO<EnemyManager>(0, "EnemyManager");
	m_enemyManager->Init();

	m_camera = NewGO<GameCamera>(0, "camera");
	m_playerUI = NewGO<PlayerUI>(0, "playerUI");
    g_camera3D->SetPosition(Vector3(-700.0f, 500.0f, 100.0f));
	// Stageを生成（NewGOはエンジンのリストに登録する関数）
	stage = NewGO<Stage>(0, "Stage");

	// 自分を親としてセットする
	stage->SetParentState(this);

	m_skillButton = NewGO<SkillButton>(0, "skillButton");
	m_skillButton->Init("Assets/UI/skill.DDS", SKILLGAUGEMAX); // アイコンとクールタイムを指定して初期化
}

void InGameNomalState::Update(Game* game) {

	StageClear();
	//ゲームオーバー、ゲームクリアのフラグが立っているかを確認して、立っていたらそれぞれのステートに遷移する
	if(m_isGameOver==true){
		m_game->ChangeState(new ResultState(ResultState::ResultType::enGameOver));
		m_isGameOver = false; // フラグをリセットしておく（次のプレイでゲームオーバーになったときに正しく遷移するように）
		return;
	}
	else if(m_isGameClear==true){
		//Boss戦のステートに遷移する予定
		m_game->ChangeState(new InGameBossState());
		m_isGameClear = false; // フラグをリセットしておく（次のプレイでゲームクリアになったときに正しく遷移するように）
		return;
	}

	//スキルゲージが100溜まったら
	if (m_player->m_playerSkillGauge >= SKILLGAUGEMAX) {
		m_isSPButtonIsReady = true; // SPボタンが押せる状態にする
	}
	else {
		m_isSPButtonIsReady = false; // SPボタンが押せない状態にする
	}
	//SPボタンが押せる状態でLB1が押されたらSPボタンを発動させる
	if (m_isSPButtonIsReady && g_pad[0]->IsTrigger(enButtonDown)) {//ボタンは変える！
		//SPボタンの発動処理をここに書く
		m_skillButton->UseSkill();//まず、スキルボタンクラス側の発動関数を呼ぶ。
		m_player->m_playerSkillGauge = 0;//プレイヤー側のスキルゲージを0にする
		m_game->PushState(new InGameRythmState());//リズムゲームのステートをPushする。これでリズムゲームのステートが表に出る。
	}
	
	m_skillButton->Update((float)m_player->m_playerSkillGauge);//スキルボタンにはプレイヤーのスキルゲージを渡す。
}

void InGameNomalState::Render(RenderContext& rc) {
	m_skillButton->Draw(rc);
}

void InGameNomalState::OnPause()
{
	//自分がNewGOしたクラスたちもDeActiveteする。
	//PlayerとStageは親管理でUpdateを止めるようにしているため、Deactivateする必要はない。
	//他のもおんなじにしてもよさそう。
	
	m_camera->Deactivate();
	m_enemyManager->Deactivate();
	m_playerUI->Deactivate();
	this->Deactivate(); // ステートを非アクティブにする
}

void InGameNomalState::OnResume()
{
	//自分がNewGOしたクラスたちもDeActiveteされているはずなので、必要に応じてそれらもActivateする
	m_camera->Activate();
	m_enemyManager->Activate();
	m_playerUI->Activate();
	this->Activate(); // ステートをアクティブにする
}

void InGameNomalState::UpdateSPButton()
{
}

void InGameNomalState::StageClear()
{
	if (CREACOUNT <= m_enemyManager->GetDeathCount()&&m_isChangeIsReady==false) {
				m_isGameClear = true;
				m_isChangeIsReady = true; // ステート遷移が発動したことを示すフラグを立てる
	}
	
}
