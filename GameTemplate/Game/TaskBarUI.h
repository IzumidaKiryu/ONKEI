#pragma once

class InGameNomalState;
class EnemyManager;

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

    SpriteRender  m_backBar;    //タスクバーの背景
    SpriteRender  m_fillBar;    //タスクバーのゲージ本体
    SpriteRender  m_partition;  //区切り用

    FontRender m_taskFont;//「敵をたくさん倒そう！」のテキスト
    FontRender m_killFont;//「倒した数：」のテキスト
	FontRender m_killEnemyFont;//現在のキル数のテキスト
	FontRender m_gameTimerFont;//残り時間のテキスト

	int m_killCount = 0; // 現在のキル数
	int m_killMax = 20;   // クリアに必要なキル数
};