#pragma once
#include "GameState.h"


//boss戦のインゲームステート
class Player;
class GameCamera;
class InGameBossState : public IGameState {
public:
    ~InGameBossState();
    void Initialize(Game* game) override;
    void Update(Game* game) override;
    void Render(RenderContext& rc) override;
	void OnPause() override; // ポーズ状態に入るときの処理
	void OnResume() override; // ポーズ状態から復帰するときの処理
private:
    ModelRender m_stageModel;
    Player* m_player;
    PhysicsStaticObject m_physicsStaticObject;
    Game* m_game;
    GameCamera* m_camera;
};