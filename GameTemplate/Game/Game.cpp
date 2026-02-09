#include "stdafx.h"
#include "Game.h"


bool Game::Start()
{
	m_modelRender.Init("Assets/2DShip/2DNewShip.tkm");
	g_camera3D->SetPosition(Vector3(-700.0f, 500.0f, 100.0f));
	g_camera3D->SetTarget(m_pos);
	
	
	return true;
}

void Game::Update()
{
	// g_renderingEngine->DisableRaytracing();
	m_modelRender.Update();
}

void Game::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}