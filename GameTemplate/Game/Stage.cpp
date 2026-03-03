#include "stdafx.h"
#include "Stage.h"
#include "InGameNomalState.h"
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
	// 親ステートが存在し、かつアクティブな時だけモデルを更新する
	   // もし「リズムゲーム中も背景を動かしたい」なら、このifを外して常にUpdateすればOKです
	if (m_parentState && m_parentState->IsActive()) {
		m_stageModel.Update();
	}
}

void Stage::Render(RenderContext& rc)
{
	m_stageModel.Draw(rc);

}
