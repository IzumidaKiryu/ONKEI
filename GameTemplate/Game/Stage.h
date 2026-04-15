#pragma once
class InGameNomalState;
class InGameBossState;  // ★追加
class Game;
class Stage :public IGameObject
{
	public:
	Stage();
	~Stage();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;
	void SetParentState(InGameNomalState* parent) { m_parentState = parent; }
	void SetParentState(InGameBossState* state) { m_bossState = state; } // ★追加
private:
	ModelRender m_stageModel;
	PhysicsStaticObject m_physicsStaticObject;
	InGameNomalState* m_inGameNormalState = nullptr;
	InGameNomalState* m_parentState = nullptr; // 親ステートへのポインタ
	InGameBossState* m_bossState = nullptr; // ★追加
};

