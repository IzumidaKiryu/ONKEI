#pragma once
#include "GameState.h"

//リザルト画面の状態を管理するクラスステート
class ResultState:public IGameState
{
public:
	enum class ResultType
	{
		enClear,
		enGameOver,
		enNum
	};
	public:
		ResultState(ResultType type);
	~ResultState() = default;
	void Initialize(Game* game) override;
	void Update(Game* game) override;
	void Render(RenderContext& rc) override;

private:
	ResultType m_resultType=ResultType::enNum;
};

