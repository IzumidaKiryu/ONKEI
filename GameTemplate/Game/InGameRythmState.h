#pragma once
#include "GameState.h"

//インゲーム中のリズムゲームの状態を管理するクラス
//こいつは他と違って現在のInGameStateにオーバーレイする形で動作する
//こいつ動作中はNomalやBossのInGameStateは一時停止。

class InGameRythmState:public IGameState
{
public:
    void Initialize(Game* game) override;
    void Update(Game* game) override;
    void Render(RenderContext& rc) override;

private:

};

