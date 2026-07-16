#pragma once
class Player;
class GameCamera :public IGameObject
{
public:
	GameCamera();
	~GameCamera();

	bool Start();
	void Update();

	//カメラを揺らす。敵を倒した瞬間などに外部から呼ぶ。
	//time:揺らす秒数、power:揺れ幅。連続で呼ばれたら強い方・長い方が優先される。
	void Shake(float time, float power);

	Vector3 m_target = Vector3::Zero;
	Vector3 m_pos = Vector3::Zero;
	Vector3 m_toCameraPos = Vector3::Zero;
	Vector3 m_cameUp = { 0.0f,1.0f,0.0f };
	Quaternion qRot = Quaternion::Identity;
	ModelRender m_mol;

	Player* m_player = nullptr;

private:
	//揺れの更新。残り時間に応じて減衰させる。
	void UpdateShake();

	Vector3 m_shakeOffset = Vector3::Zero;	//今フレームの揺れ量
	float m_shakeTime = 0.0f;				//揺れの残り時間
	float m_shakeTimeMax = 0.0f;			//減衰の基準にする揺れの総時間
	float m_shakePower = 0.0f;				//揺れ幅
};


