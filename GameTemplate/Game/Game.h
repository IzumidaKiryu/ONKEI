#pragma once  
#include <memory>  
#include <stack>  
#include "GameState.h"  

//ステート管理するゲームクラス
//ここがゲームの本体」になる
//IgameStateを使用しているクラスのインスタンスを渡して状態を変更する
//グッチのやつとかみたいにもっとスマートにできればなあと思ってるが一旦は完成が先かな。
class Game : public IGameObject {  
public:  
	~Game();
    bool Start() override;  
    void Update() override;  
    void Render(RenderContext& rc) override; // 名前空間を明示してあいまいさを解消  

    // ステート操作  
	void FarstState(IGameState* firstState); // 最初のステートをセットする関数を追加
    void ChangeState(IGameState* nextState); // 引数もポインタに
    void PushState(IGameState* overlayState);
    void PopState();
    IGameState* GetCurrentState() { return m_currentState; }

	//ヒットストップの要求。指定フレーム数だけゲーム全体の時間を止める。
	//EnemyManager等ではなくGameが持つ理由：EnemyManagerはリズムゲーム中にDeactivateされ、
	//Updateが止まって解除できなくなる（＝ゲームが永久に固まる）。Gameは誰も止めない。
	void RequestHitStop(int frames);

	//制限時間（秒）。インゲームはこの時間からカウントダウンする。
	static constexpr float GAME_TIME_LIMIT = 90.0f;

	int m_deathCount = 0; // 死亡数をカウントする変数
    int m_deathflag = 0; // 死亡フラグを追加
    float m_gameTimer = GAME_TIME_LIMIT;//残り時間（減算していく）
    float m_scoreTimer = 0.0f;//クリアした時間を取得するための変数
	int m_rythmScore = 0;//リズムゲームで稼いだスコアの累計
	int m_totalScore = 0;//合計スコア（撃破スコア＋リズムゲームスコア）

	//インゲーム開始時に、前回のプレイの記録をまとめてリセットする。
	//これを呼ばないと2回目のプレイで残り時間が0のままになり即リザルトへ飛んでしまう。
	void ResetGame() {
		m_deathCount = 0;
		m_rythmScore = 0;
		m_totalScore = 0;
		m_gameTimer = GAME_TIME_LIMIT;
		m_scoreTimer = 0.0f;
	}
private:  
	//ヒットストップの解除処理。毎フレームGame::Updateの先頭で呼ぶ。
	void UpdateHitStop();
	//ヒットストップの残りフレーム数。
	//止めている間はデルタタイムが0になるため、解除は「時間」ではなく「フレーム数」で数える。
	//時間で数えると0秒しか進まず、永久に解除されない。
	int m_hitStopFrames = 0;

    IGameState* m_currentState = nullptr; // unique_ptrをやめる
    // stackも使うなら std::stack<IGameState*> に変更
    std::stack<IGameState*> m_stateStack;

};
