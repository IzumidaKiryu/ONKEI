#pragma once

class EnemyManager;
class Player;
class EnemyAttack;

class Enemy : public IGameObject {
public:
	~Enemy();
    bool Start() override;
    void Update() override;
	void Move();// 敵の移動を管理する関数
	void Rotation();// 敵の回転を管理する関数
	void ManageState();// 敵の状態を管理する関数
	void AnimState();// アニメーションの状態を管理する関数
	void MoveState();// 敵の移動状態を管理する関数
	void Wander();// 敵の徘徊を管理する関数
	void Attack();// 攻撃する関数
	void OnDamage(int damage);// ダメージを受ける関数
    void Render(RenderContext& rc) override;
	const Vector3& GetPosition() const { return m_position; }
    
	enum EnAnimationClip {
		m_enAnimClip_Idle,   // 待機
		m_enAnimClip_Walk,   // 歩き
		m_enAnimClip_Attack, // 攻撃
		m_enAnimClip_Death,  // 死亡
		m_enAnimClip_Num,
	};
	Vector3 m_firstPos;// 敵の初期座標
	Vector3 m_position;// 敵の座標
	Vector3 m_wanderPos;//敵の徘徊時の地点
	Quaternion m_rot = Quaternion::Identity;// 敵の回転

	int m_hpMax = 50;// 敵の最大HP
	int m_hp = m_hpMax;// 敵の現在のHP

private:
    ModelRender m_modelRender;
    EnemyManager* m_enemyManager = nullptr; // 自分を管理している EnemyManager へのポインタ
    Player* m_player = nullptr; // 追いかける対象
	EnemyAttack* m_enemyAttack = nullptr; // 敵の攻撃オブジェクトへのポインタ
    CharacterController m_charaCon;
	CollisionObject* m_collisionObject = nullptr;
    AnimationClip m_animationClips[m_enAnimClip_Num];

    int m_enemyState = EnAnimationClip::m_enAnimClip_Idle; // 0:待機, 1:移動, 2:攻撃
	float m_attackTimer = 0.0f; //攻撃判定用のタイマー
	int m_enemymukiState = 0;// 敵の向きの状態を管理する変数。0:プレイヤーの正面, 1:プレイヤーの背面
	bool m_hasDealtDamage = false;// 攻撃が当たったかどうかを管理するフラグ
	bool m_wanderPosSet = false;// 徘徊地点が設定されているかどうかを管理するフラグ
	bool m_attackRot = false;//攻撃の向きのフラグ
	Vector3 m_attackDir = Vector3::Zero; // ★追加：予兆完了時の方向を保存する変数
	// HPバー用のメンバを追加
	SpriteRender m_hpBarSprite;      // HPの緑色部分
	SpriteRender m_hpBarBackSprite;  // HPの背景（赤や黒）

	// HPバーを更新する内部関数
	void UpdateHPBar();
};