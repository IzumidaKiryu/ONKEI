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

    int m_deathflag = 0; // 死亡フラグを追加
    float m_gameTimer = 0.0f;//プレイ時間加算用変数
    float m_scoreTimer = 0.0f;//クリアした時間を取得するための変数
private:  
    IGameState* m_currentState = nullptr; // unique_ptrをやめる
    // stackも使うなら std::stack<IGameState*> に変更
    std::stack<IGameState*> m_stateStack;

};
