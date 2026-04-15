#include "stdafx.h"
#include "Stage.h"
#include "InGameNomalState.h"
#include "InGameBossState.h"
Stage::Stage()
{
}

Stage::~Stage()
{
}

bool Stage::Start()
{
	m_stageModel.Init("Assets/modelData/RPGGround/bg.tkm");
	m_physicsStaticObject.CreateFromModel(m_stageModel.GetModel(), m_stageModel.GetModel().GetWorldMatrix());

	return true;
}

void Stage::Update()
{
    // 1. ノーマルステートもボスステートもどちらもいない場合、
    // または、いる方のステートがアクティブでないなら何もしない

    bool isNormalActive = m_parentState && m_parentState->IsActive();
    bool isBossActive = m_bossState && m_bossState->IsActive();

    // どちらのステートもアクティブでないならリターン
    if (!isNormalActive && !isBossActive) {
        return;
    }
		m_stageModel.Update();
	
}

void Stage::Render(RenderContext& rc)
{
	m_stageModel.Draw(rc);

}
