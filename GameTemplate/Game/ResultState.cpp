#include "stdafx.h"
#include "ResultState.h"
#include "Game.h"
#include "TitleState.h"
#include "sound/SoundEngine.h"

ResultState::ResultState(ResultType type)
{
    m_resultType = type;
}

void ResultState::Initialize(Game* game)
{
	// ゲームクラスのポインタを保持
	m_game = game;

	m_killCount = m_game->m_deathCount; // 前ステージで倒した敵の数を取得

    // 1. 背景の読み込み
    m_BackGroundSprite.Init("Assets/UI/result.DDS", 1920, 1080);

    // 2. 青い帯の初期化 (白い画像を青く着色して半透明にする)
    m_blueBandSprite.Init("Assets/sprite/white.DDS", 1920, 350);
    m_blueBandSprite.SetMulColor({ 0.0f, 0.2f, 0.6f, 0.5f }); // GV風の半透明紺色

    // 3. スコアとタイムの目標値を設定 (本来は前ステージのマネージャーから取得)
    m_targetScore = 125000.0f;
	//m_targetClearTime = m_game->m_scoreTimer; // 前ステージで記録したプレイ時間を使用

	m_resultSprite.Init("Assets/UI/resultText.DDS", 1920, 1080);
	m_resultSprite.SetPosition(Vector3(-750.0f, 500.0f, 0.0f));
	m_resultSprite.SetScale(Vector3(0.5f, 0.5f, 1.0f));
	m_resultSprite.Update();

    //※あらかじめランクの画像を初期化
    m_rankSprite.Init("Assets/UI/resultRankA.DDS", 1920, 1080);
    m_rankSprite.SetPosition(Vector3(650.0f, 800.0f, 0.0f));

	//文字の表示
	m_ButtonFont.SetText(L"Press A Button");
	m_ButtonFont.SetPosition(Vector3(-200.0f, -250.0f, 0.0f));
    m_ButtonFont.SetColor(m_fontColor);

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
        //if (m_displayClearTime < m_targetClearTime) {
        //    m_displayClearTime += (m_targetClearTime - m_displayClearTime) * 0.1f + 0.05f;
        //    if (m_displayClearTime >= m_targetClearTime) {
        //        m_displayClearTime = m_targetClearTime;
        //        ScoreRank(); // ここでランク判定し、正しい画像に Init される
        //        // ここでタイム確定音！
        //        m_currentPhase = Phase::enRankDisplay; // 次の演出へ
        //    }
        //}

		// キル数を表示する場合の処理
        if (m_killCountFontDraw == true) {
            ScoreRank(); // ここでランク判定し、正しい画像に Init される
            // ここでタイム確定音！
            m_currentPhase = Phase::enRankDisplay; // 次の演出へ
        }
        break;

    case Phase::enRankDisplay:
        // --- 3. ランク表示 ---
        if (!m_isRankSoundPlayed) {
            // ★演出が終わった瞬間にスコアが確定しているので、ここで再判定＆差し替え！
            ScoreRank();
            m_isRankSoundPlayed = true;
        }
        // ここでは少しの間（ウェイト）を置いたり、
        // ランク画像を「ドォン！」と出す準備をします。
        // とりあえず今回は即座に入力待ちへ
        m_currentPhase = Phase::enWaitInput;
        break;

    case Phase::enWaitInput:
        // --- 4. 入力待ち ---
        if (g_pad[0]->IsTrigger(enButtonA) && m_isChangeFlag == false) {
			//ボタンを押したときの効果音を鳴らす
            g_soundEngine->ResistWaveFileBank(6, "Assets/sound/TitleBGM.wav");
            m_tapSound = NewGO<SoundSource>(6); 
            m_tapSound->Init(6);
            m_tapSound->Play(false);
            // 次のシーンへ
            m_game->ChangeState(new TitleState());
            m_isChangeFlag = true;
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
        /*int totalSec = (int)m_displayClearTime;
        int min = totalSec / 60;
        int sec = totalSec % 60;
        int milli = (int)((m_displayClearTime - totalSec) * 100);

        wchar_t timeStr[512];
        swprintf_s(timeStr, L"CLEAR TIME     %02d : %02d : %02d", min, sec, milli);

        m_clearTimeFont.SetText(timeStr);
        m_clearTimeFont.SetPosition({ -650.0f, 0.0f, 0.0f });
        m_clearTimeFont.SetScale(1.0f);
        m_clearTimeFont.Draw(rc);*/

		// キル数を描画
		wchar_t killCountStr[256];
		swprintf_s(killCountStr,256, L"KILL COUNT     %d", int(m_killCount));
		m_killCountFont.SetText(killCountStr);
		m_killCountFont.SetPosition({ -650.0f, -50.0f, 0.0f });
		m_killCountFont.SetScale(1.0f);
		m_killCountFont.Draw(rc);
        //カウントのリセット
		m_killCount = 0;
		m_game->m_deathCount = 0;
		m_killCountFontDraw = true; // キル数を描画したことを記録
    }

    // ランク表示フェーズ以降ならランク画像を描画
    if (m_currentPhase >= Phase::enRankDisplay) {
        m_rankSprite.SetPosition({ 0.0f, 0.0f, 0.0f });
        m_rankSprite.Draw(rc);
		m_ButtonFont.Draw(rc);
    }
}

void ResultState::ScoreRank()
{
	// ゲームオーバーならAランク
    if (m_resultType == ResultType::enGameOver) {
        m_rank = Rank::enA;
    }
	//雑魚を100体以上倒していたらS+ランク
    else if (m_killCount >= 100){
        m_rank = Rank::enS_Plus;
    }
	// 50体以上70体未満ならSランク
    else if (70 > m_killCount && m_killCount >= 50) {
        m_rank = Rank::enS;
    }
	// 50体未満ならAランク
    else { 
        m_rank = Rank::enA;
    }

    // スプライトのパスだけを決める
    const char* path = "Assets/UI/resultRankA.DDS";
    switch (m_rank)
    {
    case Rank::enS_Plus: 
        path = "Assets/UI/resultRankSS.DDS"; 
        break;
    case Rank::enS:      
        path = "Assets/UI/resultRankS.DDS";  
        break;
    case Rank::enA:      
        path = "Assets/UI/resultRankA.DDS";  
        break;
    }

    // 既に Init 済みでも、再度 Init すれば画像が差し替わる！
    m_rankSprite.Init(path, 1920, 1080);
    m_rankSprite.SetPosition(Vector3(650.0f, 800.0f, 0.0f));
}
