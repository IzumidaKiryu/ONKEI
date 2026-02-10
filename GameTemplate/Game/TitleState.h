#pragma once
#include "GameState.h"

class TitleState : public IGameState {
public:
    void Initialize(Game* game) override;
    void Update(Game* game) override;
    void Render(RenderContext& rc) override;
private:
	SpriteRender m_titleSprite;
	Game* m_game;
};