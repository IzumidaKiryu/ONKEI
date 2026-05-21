#pragma once

class TaskBarUI;

class NomalTaskUI : public IGameObject
{
public:
	NomalTaskUI();
	~NomalTaskUI();
	void Init();
	void Update();
	void Render(RenderContext& rc);


	TaskBarUI* m_taskBarUI = nullptr; // タスクバーUIの参照

	FontRender m_taskFont; // タスクの説明を表示するためのフォント
};

