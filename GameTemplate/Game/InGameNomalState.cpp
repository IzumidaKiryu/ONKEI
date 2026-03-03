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
namespace
{
	int CREACOUNT=5; // 軟体倒したらクリアにするか変数
}

InGameNomalState::~InGameNomalState() {
	DeleteGO(m_player);
	DeleteGO(m_camera);
	DeleteGO(m_enemyManager);
	DeleteGO(m_playerUI);
}

void InGameNomalState::Initialize(Game* game) {
    m_game = game;
	m_player = NewGO<Player>(0,"player");
	m_enemyManager=NewGO<EnemyManager>(0, "EnemyManager");
	m_enemyManager->Init();
	m_stageModel.Init("Assets/modelData/RPGGround/bg.tkm");
	m_physicsStaticObject.CreateFromModel(m_stageModel.GetModel(), m_stageModel.GetModel().GetWorldMatrix());
	m_camera = NewGO<GameCamera>(0, "camera");
	m_playerUI = NewGO<PlayerUI>(0, "playerUI");
    g_camera3D->SetPosition(Vector3(-700.0f, 500.0f, 100.0f));
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
	//SPボタンが押せる状態でLB1が押されたらSPボタンを発動させる
	if (m_isSPButtonIsReady && g_pad[0]->IsTrigger(enButtonLB1)) {//ボタンは変える！
		//SPボタンの発動処理をここに書く
		m_game->PushState(new InGameRythmState());
		m_isSPButtonIsReady = false; // SPボタンを使用したので、再度押せるようになるまでフラグをfalseにする
	}

	m_stageModel.Update();
}

void InGameNomalState::Render(RenderContext& rc) {
	m_stageModel.Draw(rc);
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
