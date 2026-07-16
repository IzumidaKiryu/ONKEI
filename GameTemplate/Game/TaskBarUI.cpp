#include "stdafx.h"
#include "TaskBarUI.h"
#include "InGameNomalState.h"
#include "EnemyManager.h"
#include "Game.h"

namespace
{
    //ゲージのピボット。
    const Vector2 GAGE_PIVOT = { 0.0f, 0.5f };
    // タスクバーの表示位置
    const Vector3 UI_POSITION = { 500.0f, 300.0f, 0.0f }; // 左上あたり
	// 区切りの表示位置
	const Vector3 PARTITION_POSITION = { 550.0f, 340.0f, 0.0f }; // タスクバーの上あたり
	//フォントの表示位置
	const Vector3 FONT_POSITION = { 400.0f, 400.0f, 0.0f }; // 区切りの上あたり
    // 色設定
    const Vector4 COLOR_YELLOW = { 1.0f, 1.0f, 0.0f, 1.0f }; //黄色
    const Vector4 COLOR_GREEN = { 0.0f, 1.0f, 0.0f, 1.0f }; //緑
    const Vector4 COLOR_WHITE = { 1.0f, 1.0f, 1.0f, 1.0f }; //白色
	const Vector4 COLOR_BLACK = { 0.0f, 0.0f, 0.0f, 1.0f }; //灰色
	const Vector4 COLOR_RED = { 1.0f, 0.2f, 0.2f, 1.0f }; //赤色（ラッシュ中の警告用）

    // タスクの説明の表示位置
    const Vector3 TASK_FONT_POSITION = { 350.0f, 400.0f, 0.0f }; // タスクバーの上あたり
    // タスクの説明のフォントサイズ
    const float FONT_SIZE = 1.0f;

    // 進捗：の表示位置
    const Vector3 KILL_FONT_POSITION = { 300.0f,325.0f,0.0f };
    //　進捗：のフォントサイズ
    const float KILL_SIZE = 1.0f;

	// 現在のキル数の表示位置
	const Vector3 KILL_ENEMY_FONT_POSITION = { 400.0f,325.0f,0.0f };

	// 残り時間の表示位置
	const Vector3 GAME_TIMER_FONT_POSITION = { 400.0f,275.0f,0.0f };

}

TaskBarUI::TaskBarUI() {

}

TaskBarUI::~TaskBarUI() {

}


void TaskBarUI::Init()
{
    // 背景バーの初期化
    m_backBar.Init("Assets/UI/bar.DDS", 300, 40);
    m_backBar.SetPosition(UI_POSITION);
    m_backBar.SetMulColor(COLOR_BLACK);
    m_backBar.SetPivot(GAGE_PIVOT); // 左端を起点にする

    // ゲージ本体の初期化
    m_fillBar.Init("Assets/UI/bar.DDS", 300, 40);
    m_fillBar.SetPosition(UI_POSITION);
    m_fillBar.SetMulColor(COLOR_YELLOW);
    m_fillBar.SetPivot(GAGE_PIVOT); // 左端を起点にする

    // 区切りの初期化
    m_partition.Init("Assets/sprite/white.DDS", 500, 10);
    m_partition.SetPosition(PARTITION_POSITION);
    m_partition.SetMulColor(COLOR_WHITE);

    //タスクの説明の初期化
    wchar_t taskDescription[256];
    std::wstring hff = L"敵をたくさん倒そう！";
    m_taskFont.SetText(hff.c_str());
    m_taskFont.SetPosition(TASK_FONT_POSITION);
    m_taskFont.SetScale(FONT_SIZE);
    m_taskFont.SetColor(COLOR_GREEN);

    //倒した数：の初期化
    wchar_t killDescription[256];
    std::wstring kill = L"倒した数 ";
    m_killFont.SetText(kill.c_str());
    m_killFont.SetPosition(KILL_FONT_POSITION);
    m_killFont.SetScale(KILL_SIZE);
    m_killFont.SetColor(COLOR_WHITE);

	//現在のキル数の初期化
	m_killEnemyFont.SetText(L"0");
	m_killEnemyFont.SetPosition(KILL_ENEMY_FONT_POSITION);
	m_killEnemyFont.SetScale(KILL_SIZE);
	m_killEnemyFont.SetColor(COLOR_WHITE);

	// 残り時間の初期化
	m_gameTimerFont.SetText(L"残り：");
	m_gameTimerFont.SetPosition(GAME_TIMER_FONT_POSITION);
	m_gameTimerFont.SetScale(KILL_SIZE);
	m_gameTimerFont.SetColor(COLOR_WHITE);

    // 参照を取得
    m_enemyManager = FindGO<EnemyManager>("EnemyManager");
	m_gameRef = FindGO<Game>("game");
}

void TaskBarUI::Update()
{
    
    if (m_enemyManager == nullptr || m_gameRef == nullptr) return;

    //////////////// タスクバーのゲージ処理 ////////////////////////////////////////

    // 1. 現在の撃破数と残り時間を取得
    m_killCount = m_enemyManager->GetDeathCount();
	m_remainTime = m_gameRef->m_gameTimer;

	// 2. 制限時間制なので、ゲージは「残り時間」の割合を表す
	float wari = m_remainTime / Game::GAME_TIME_LIMIT;
	if (wari < 0.0f) wari = 0.0f;
	if (wari > 1.0f) wari = 1.0f;

	// 3. ゲージのスケールを更新
    m_fillBar.SetScale({ wari, 1.0f, 1.0f });

	// 4. ラッシュ中はゲージと残り時間を赤くして、残り30秒を切ったことを知らせる
	const bool isRush = m_enemyManager->IsRushMode();
	m_fillBar.SetMulColor(isRush ? COLOR_RED : COLOR_YELLOW);

	// 5. 現在のキル数をフォントに反映
	wchar_t killEnemyStr[256];
	swprintf_s(killEnemyStr, L"%d", m_killCount);
	m_killEnemyFont.SetText(killEnemyStr);

    // 6. ゲーム時間の表示を更新（切り上げて1〜90と表示されるようにする）
	wchar_t gameTimerStr[256];
	swprintf_s(gameTimerStr, L"残り：%d秒", (int)ceil(m_remainTime));
	m_gameTimerFont.SetText(gameTimerStr);
	m_gameTimerFont.SetColor(isRush ? COLOR_RED : COLOR_WHITE);

	// 更新処理
    m_backBar.Update();
    m_fillBar.Update();
	m_partition.Update();
}

void TaskBarUI::Render(RenderContext& rc)
{
    m_backBar.Draw(rc);
    m_fillBar.Draw(rc);
	m_partition.Draw(rc);
    m_taskFont.Draw(rc);
    m_killFont.Draw(rc);
	m_killEnemyFont.Draw(rc);
	m_gameTimerFont.Draw(rc);
}