#pragma once
class Player;
class GameCamera :public IGameObject
{
public:
	GameCamera();
	~GameCamera();

	bool Start();
	void Update();

	Vector3 m_target = Vector3::Zero;
	Vector3 m_pos = Vector3::Zero;
	Vector3 m_toCameraPos = Vector3::Zero;
	Vector3 m_cameUp = { 0.0f,1.0f,0.0f };
	Quaternion qRot = Quaternion::Identity;
	ModelRender m_mol;

	Player* m_player = nullptr;
};


