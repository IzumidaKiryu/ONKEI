#include "stdafx.h"
#include "StageLight.h"

namespace
{
	//ライトの色。
	const Vector3 LIGHT_COLOR = { 1.0f, 1.0f, 1.0f };
	//ライトの強さ。
	const float LIGHT_POWER = 1.0f;
	//ライトの減衰距離。
	const float LIGHT_ATTENUATION = 10.0f;
}

StageLight::StageLight()
{

}

StageLight::~StageLight()
{

}

void StageLight::Init()
{
	//ライトの初期化。
	m_pointLight.Init();
	m_pointLight.SetColor(LIGHT_COLOR);
	m_pointLight.SetRange(LIGHT_ATTENUATION);
	m_pointLight.SetAffectPowParam(LIGHT_POWER);

}

void StageLight::Update()
{
	m_pointLight.SetPosition(m_pos);
	m_pointLight.Update();
}

void StageLight::Render(RenderContext&rc)
{

}
