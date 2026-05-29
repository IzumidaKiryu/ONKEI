#include "stdafx.h"
#include "TaskBarUI.h"
#include "InGameNomalState.h"
#include "EnemyManager.h"
#include "NomalTaskUI.h"

namespace
{
    //ゲージのピボット。
    const Vector2 GAGE_PIVOT = { 0.0f, 0.5f };
    // タスクバーの表示位置
    const Vector3 UI_POSITION = { 400.0f, 300.0f, 0.0f }; // 左上あたり
	// 区切りの表示位置
	const Vector3 PARTITION_POSITION = { 550.0f, 340.0f, 0.0f }; // タスクバーの上あたり
	//フォントの表示位置
	const Vector3 FONT_POSITION = { 400.0f, 400.0f, 0.0f }; // 区切りの上あたり
    // 色設定
    const Vector4 COLOR_YELLOW = { 1.0f, 1.0f, 0.0f, 1.0f }; //黄色
    const Vector4 COLOR_GREEN = { 0.0f, 1.0f, 0.0f, 1.0f }; //緑
    const Vector4 COLOR_WHITE = { 1.0f, 1.0f, 1.0f, 1.0f }; //白色
	const Vector4 COLOR_BLACK = { 0.0f, 0.0f, 0.0f, 1.0f }; //灰色
}

TaskBarUI::TaskBarUI() {

}

TaskBarUI::~TaskBarUI() {

    DeleteGO(m_taskUI);
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

    // タスク用テキストの生成
    m_taskUI = NewGO<NomalTaskUI>(0, "nomalTaskUI");
    m_taskUI->Init();

    // 参照を取得
    m_enemyManager = FindGO<EnemyManager>("EnemyManager");
}

void TaskBarUI::Update()
{
    
    if (m_enemyManager == nullptr) return;
    
    //////////////// タスクバーのゲージ処理 ////////////////////////////////////////

    // 1. 現在の撃破数を取得
    m_killCount = static_cast<float>(m_enemyManager->m_deathCount);
	m_killMax = 20;

	// 2. 撃破数の割合を計算
	float wari = (float)m_killCount / (float)m_killMax;
	Vector3 scale = { wari, 1.0f, 1.0f };
    scale.x = wari;

	// 3. ゲージのスケールを更新
    m_fillBar.SetScale(scale);

	// 4. 背景バーよりもゲージ本体の方が大きくならないようにする
    if (m_killCount >= m_killMax) {

		m_killCount = m_killMax;

    }

	// 更新処理
    m_backBar.Update();
    m_fillBar.Update();
	m_partition.Update();

    //m_killEnemyFontRender.SetPosition(FONT_POSITION);// フォントの位置をタスクバーの上に設定

}

void TaskBarUI::Render(RenderContext& rc)
{
    m_backBar.Draw(rc);
    m_fillBar.Draw(rc);
	m_partition.Draw(rc);
	/*m_missionKill.Draw(rc);*/
}