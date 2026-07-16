#pragma once

class InGameNomalState;
class EnemyManager;
class Game;

class TaskBarUI : public IGameObject
{
public:
    //メンバ関数
    TaskBarUI();
    ~TaskBarUI();
	void Init();
    void Update();
    void Render(RenderContext& rc);

private:
    // メンバ変数
    InGameNomalState* m_nomalState = nullptr; // クリア条件の参照
    EnemyManager* m_enemyManager = nullptr;   // 現在のキル数の参照
    Game* m_gameRef = nullptr;                // 残り時間の参照

    SpriteRender  m_backBar;    //タスクバーの背景
    SpriteRender  m_fillBar;    //タスクバーのゲージ本体
    SpriteRender  m_partition;  //区切り用

    FontRender m_taskFont;//「敵をたくさん倒そう！」のテキスト
    FontRender m_killFont;//「倒した数 12」のテキスト（ラベルと数値をまとめて描く）
	FontRender m_gameTimerFont;//「残り：60秒」のテキスト
	FontRender m_chainFont;//「12 CHAIN x1.9」のテキスト。連続撃破中だけ出す。

	int m_killCount = 0; // 現在のキル数
	float m_remainTime = 0.0f; // 残り時間（秒）
	int m_chainCount = 0; // 現在の連続撃破数
};