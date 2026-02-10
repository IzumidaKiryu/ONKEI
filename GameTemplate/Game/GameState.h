#pragma once

class Game;


class IGameState:public IGameObject {
public:
    virtual ~IGameState() = default;
    virtual void Initialize(Game* game) = 0;
    virtual void Update(Game* game) = 0;
    virtual void Render(RenderContext& rc) = 0;
};