#include "stdafx.h"
#include "ResultState.h"
#include "Game.h"
#include "TitleState.h"

ResultState::ResultState(ResultType type)
{
    m_resultType = type;
}

void ResultState::Initialize(Game* game)
{
    // 1. 背景の読み込み
    m_BackGroundSprite.Init("Assets/UI/result.DDS", 1920, 1080);

    // 2. 青い帯の初期化 (白い画像を青く着色して半透明にする)
    m_blueBandSprite.Init("Assets/sprite/white.DDS", 1920, 350);
    m_blueBandSprite.SetMulColor({ 0.0f, 0.2f, 0.6f, 0.5f }); // GV風の半透明紺色

    // 3. スコアとタイムの目標値を設定 (本来は前ステージのマネージャーから取得)
    m_targetScore = 125000.0f;
    m_targetClearTime = 262.03f; // 4分22秒03

	m_resultSprite.Init("Assets/UI/resultText.DDS", 1920, 1080);
	m_resultSprite.SetPosition(Vector3(-750.0f, 500.0f, 0.0f));
	m_resultSprite.SetScale(Vector3(0.5f, 0.5f, 1.0f));
	m_resultSprite.Update();

	//ランク画像の初期化
	m_rankSprite.Init("Assets/UI/resultRankSS.DDS", 1920, 1080);
    m_rankSprite.SetPosition(Vector3(650.0f, 800.0f, 0.0f));
	m_rankSprite.SetScale(Vector3(1.0f, 1.0f, 1.0f));
	m_rankSprite.Update();

	//文字の表示
	m_ButtonFont.SetText(L"Press A Button");
	m_ButtonFont.SetPosition(Vector3(-200.0f, -250.0f, 0.0f));
    m_ButtonFont.SetColor(m_fontColor);


    m_displayScore = 0.0f;
    m_displayClearTime = 0.0f;
    m_isCounting = true;
}

void ResultState::Update(Game* game)
{
    m_game = game;
    switch (m_currentPhase)
    {
    case Phase::enScoreCount:
        // --- 1. スコア加算 ---
        if (m_displayScore < m_targetScore) {
            m_displayScore += (m_targetScore - m_displayScore) * 0.1f + 111.0f;
            if (m_displayScore >= m_targetScore) {
                m_displayScore = m_targetScore;
                // ここでスコア確定音！
                m_currentPhase = Phase::enTimeCount; // 次の演出へ
            }
        }
        break;

    case Phase::enTimeCount:
        // --- 2. タイム加算 ---
        if (m_displayClearTime < m_targetClearTime) {
            m_displayClearTime += (m_targetClearTime - m_displayClearTime) * 0.1f + 0.05f;
            if (m_displayClearTime >= m_targetClearTime) {
                m_displayClearTime = m_targetClearTime;
                // ここでタイム確定音！
                m_currentPhase = Phase::enRankDisplay; // 次の演出へ
            }
        }
        break;

    case Phase::enRankDisplay:
        // --- 3. ランク表示 ---
        // ここでは少しの間（ウェイト）を置いたり、
        // ランク画像を「ドォン！」と出す準備をします。
        // とりあえず今回は即座に入力待ちへ
        m_currentPhase = Phase::enWaitInput;
        break;

    case Phase::enWaitInput:
        // --- 4. 入力待ち ---
        if (g_pad[0]->IsTrigger(enButtonA)) {
            // 次のシーンへ
            m_game->ChangeState(new TitleState());

        }
        break;
    }

	m_rankSprite.Update();

    FontFade();
}

void ResultState::FontFade()
{
    if (!m_isFontFade)
    {
        //フォントのフェードインの処理。
        m_fontColor.r += 0.01f * m_timer / m_maxTitleTime;
        m_fontColor.g += 0.01f * m_timer / m_maxTitleTime;
        m_fontColor.b += 0.01f * m_timer / m_maxTitleTime;
        m_fontColor.a += 0.01f * m_timer / m_maxTitleTime;

        m_ButtonFont.SetColor(m_fontColor);

        if (m_fontColor.a >= 1.0f)
        {
            m_isFontFade = true;
            m_fontColor.r = 1.0f;
            m_fontColor.g = 1.0f;
            m_fontColor.b = 1.0f;
            m_fontColor.a = 1.0f;

            m_ButtonFont.SetColor(m_fontColor);
            m_timer = 0.0f;

        }
    }
    else if (m_isFontFade)
    {
        //フォントのフェードアウトの処理。
        m_fontColor.r -= 0.01f * m_timer / m_maxTitleTime;
        m_fontColor.g -= 0.01f * m_timer / m_maxTitleTime;
        m_fontColor.b -= 0.01f * m_timer / m_maxTitleTime;
        m_fontColor.a -= 0.01f * m_timer / m_maxTitleTime;

        m_ButtonFont.SetColor(m_fontColor);

        if (m_fontColor.a <= 0.0f)
        {
            m_isFontFade = false;
            m_fontColor.r = 0.0f;
            m_fontColor.g = 0.0f;
            m_fontColor.b = 0.0f;
            m_fontColor.a = 0.0f;

            m_ButtonFont.SetColor(m_fontColor);
            m_timer = 0.0f;
        }
    }
    m_ButtonFont.SetColor(m_fontColor);
}

void ResultState::Render(RenderContext& rc)
{
    m_BackGroundSprite.Draw(rc);
    m_blueBandSprite.SetPosition({ 0.0f, -50.0f, 0.0f });
    m_blueBandSprite.Draw(rc);
	m_resultSprite.Draw(rc);

    // スコアは最初から出す
    wchar_t scoreStr[512];
    swprintf_s(scoreStr, L"STAGE SCORE   %09d", (int)m_displayScore);
    m_totalScoreFont.SetText(scoreStr);
    m_totalScoreFont.SetPosition({ -650.0f, 20.0f, 0.0f });
    m_totalScoreFont.Draw(rc);

    // タイム演出が始まってから（または終わってから）タイムを描画
    if (m_currentPhase >= Phase::enTimeCount) {
        int totalSec = (int)m_displayClearTime;
        int min = totalSec / 60;
        int sec = totalSec % 60;
        int milli = (int)((m_displayClearTime - totalSec) * 100);

        wchar_t timeStr[512];
        swprintf_s(timeStr, L"CLEAR TIME     %02d : %02d : %02d", min, sec, milli);

        m_clearTimeFont.SetText(timeStr);
        m_clearTimeFont.SetPosition({ -650.0f, -120.0f, 0.0f });
        m_clearTimeFont.Draw(rc);
    }

    // ランク表示フェーズ以降ならランク画像を描画
    if (m_currentPhase >= Phase::enRankDisplay) {
        m_rankSprite.SetPosition({ 0.0f, 0.0f, 0.0f });
        m_rankSprite.Draw(rc);
		m_ButtonFont.Draw(rc);
    }
}