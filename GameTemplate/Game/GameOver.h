#pragma once

class TitleState;

class GameOver :public IGameObject
{
public:
	GameOver();
	~GameOver();
	void Init();
	void Update();
	void Render(RenderContext& rc);

private:
	TitleState* m_titleState = nullptr;
	SpriteRender m_sprite;
	FontRender m_fontRender;
};

