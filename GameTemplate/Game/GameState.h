#pragma once

class Game;


class IGameState:public IGameObject {
public:
    virtual ~IGameState() = default;
    virtual void Initialize(Game* game) = 0;
    virtual void Update(Game* game) = 0;
    virtual void Render(RenderContext& rc) = 0;
    // --- 追加：ポーズ制御 ---
    // 下に隠れるときに呼ばれる
    virtual void OnPause() {}
    // 上のステートが消えて自分が表に出るときに呼ばれる
    virtual void OnResume() {}
};