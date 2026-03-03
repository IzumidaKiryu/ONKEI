#pragma once
class InGameNomalState;
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
private:
	ModelRender m_stageModel;
	PhysicsStaticObject m_physicsStaticObject;
	InGameNomalState* m_inGameNormalState = nullptr;
	InGameNomalState* m_parentState = nullptr; // 親ステートへのポインタ
};

