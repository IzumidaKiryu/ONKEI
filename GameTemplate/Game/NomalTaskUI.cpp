#include "stdafx.h"
#include "NomalTaskUI.h"
#include "TaskBarUI.h"

namespace {

	// タスクの説明の表示位置
	const Vector3 TASK_FONT_POSITION = { 400.0f, 400.0f, 0.0f }; // タスクバーの上あたり
	// タスクの説明のフォントサイズ
	const float FONT_SIZE = 1.0f;
	// タスクの説明の色
	const Vector4 FONT_GREEN = { 1.0f, 1.0f, 0.0f, 1.0f }; // 緑色

}

NomalTaskUI::NomalTaskUI() {

}

NomalTaskUI::~NomalTaskUI() {

}


void NomalTaskUI::Init()
{
	//m_taskBarUI = FindGO<TaskBarUI>("TaskBarUI");

	//タスクの説明の初期化
	wchar_t taskDescription[256];
	std::wstring hff = L"敵をたくさん倒そう！";
	m_taskFont.SetText(hff.c_str());
	m_taskFont.SetPosition(TASK_FONT_POSITION);
	m_taskFont.SetScale(FONT_SIZE);
	m_taskFont.SetColor(FONT_GREEN);

	//m_taskFont.SetText(L"敵を全て倒そう！");
	//m_taskFont.SetPosition(Vector)

}

void NomalTaskUI::Update() {

	//更新
	m_taskFont.SetPosition(TASK_FONT_POSITION);

}

void NomalTaskUI::Render(RenderContext& rc) {
	m_taskFont.Draw(rc);
}
