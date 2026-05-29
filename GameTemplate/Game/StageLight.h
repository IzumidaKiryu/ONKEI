#pragma once
class StageLight:public IGameObject
{
public:
	StageLight();
	~StageLight();
	void Init();
	void Update();
	void Render(RenderContext&rc);

private:

	Vector3 m_firstPos = Vector3::Zero;
	Vector3 m_pos = m_firstPos;

	PointLight m_pointLight;


};

