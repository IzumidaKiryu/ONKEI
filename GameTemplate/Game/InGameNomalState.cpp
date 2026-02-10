#include "stdafx.h"
#include "InGameNomalState.h"
#include "Game.h"
#include "Player.h"
#include "GameCamera.h"
#include "TitleState.h"

InGameNomalState::~InGameNomalState() {
	DeleteGO(m_player);
	DeleteGO(m_camera);
}

void InGameNomalState::Initialize(Game* game) {
    m_game = game;
	m_player = NewGO<Player>(0,"player");
	m_stageModel.Init("Assets/Karimodel/stera.tkm");
	m_physicsStaticObject.CreateFromModel(m_stageModel.GetModel(), m_stageModel.GetModel().GetWorldMatrix());
	m_camera = NewGO<GameCamera>(0, "camera");

    g_camera3D->SetPosition(Vector3(-700.0f, 500.0f, 100.0f));
}

void InGameNomalState::Update(Game* game) {

	
	m_stageModel.Update();
}

void InGameNomalState::Render(RenderContext& rc) {
	m_stageModel.Draw(rc);
}