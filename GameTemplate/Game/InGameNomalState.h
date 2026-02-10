#pragma once
#include "GameState.h"


//雑魚敵戦のインゲームステート
class Player;
class GameCamera;
class EnemyManager;
class InGameNomalState : public IGameState {
public:
	~InGameNomalState();
    void Initialize(Game* game) override;
    void Update(Game* game) override;
    void Render(RenderContext& rc) override;
	//クラス固有の関数

private:
	Game* m_game;//切り替えに使用(もっといい方法ある気がする...)
	//クラス固有の変数
	ModelRender m_stageModel;
	Player* m_player;
    PhysicsStaticObject m_physicsStaticObject;
   
	GameCamera* m_camera;
	EnemyManager* m_enemyManager;
};