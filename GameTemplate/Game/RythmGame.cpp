#include "stdafx.h"
#include "RythmGame.h"
#include "ReadJSON.h"
#include "Note.h"
#include "fstream"
#include "sound/SoundEngine.h"
#include "HitEffect.h"
#include "k2EngineLowPreCompile.h"



#ifndef RANDOM_FLOAT
#define RANDOM_FLOAT(min, max) ((min) + ((float)rand() / RAND_MAX) * ((max) - (min)))
#endif


namespace {
	const Vector3 FARSTNORTPOS_R = Vector3(150.0f, 600.0f, 0.0f);
	const Vector3 FARSTNORTPOS_L = Vector3(-150.0f, 600.0f, 0.0f);
	const int MAX_ONE_NOTE_SCORE = 50000;//最大スコア。(絆レベルによる上限解放及びアクセサリー無しの初期の上限を採用中)
	//仮の数値たちはあとでJsonファイルから読み込むようにする。
	const float NOTE_DAMAGEKARI = 752.0f;//ノーツ１個のダメージ量仮。
	const float SP_INCREASEKARI = 200.0f;//SPゲージの増加量仮(200が本家スクスタの基本値)。
	const float HP_MAXKARI = 189000.0f;//HPの最大値。
	const float VOLTAGE_MAXKARI = 7120000;//ボルテージの最大値。
}

RythmGame::RythmGame() {
	m_json = NewGO<ReadJSON>(0, "ReadJson");//ReadJSONのインスタンス生成

	//srand(time(nullptr));
}


void RythmGame::CalcMaxCombo()
{
	if (m_comboCount < 0 || m_comboCount>9999) return;//不正な値は無視。
	if (m_maxCombo < 0 || m_maxCombo>9999)return;
	if (m_comboCount > m_maxCombo) {
		m_maxCombo = m_comboCount;
	}
}

//スコア計算式は（https://www.youtube.com/watch?v=MkJFDHHXTw8）を参考にしました。
void RythmGame::CalcScore(int judgeType, int staminaType, bool isAC, bool isSP)
{
	// --- クリティカルのランダム抽選 ---
	int criLevel = 0;
	int r = rand() % 100;
	if (r < 50) criLevel = 1; // 50%で1.5倍、などのシンプルな抽選



	// 1. コンボ倍率 (画像の内容)
	const float combomagnification = [this]() {
		if (this->m_comboCount >= 71) return 1.05f;
		if (this->m_comboCount >= 51) return 1.03f;
		if (this->m_comboCount >= 31) return 1.02f;
		if (this->m_comboCount >= 11) return 1.01f;
		return 1.00f;
		}();

	// 2. 判定倍率 (4段階: 0, 1.0, 1.1, 1.2)PEFECT->1.2,GREAT->1.1,GOOD->1.0,MISS->0.0
	static const float judgeTable[] = { 1.2f, 1.1f, 1.0f, 0.0f };
	const float judgementmagnification = judgeTable[std::clamp(judgeType, 0, 3)];

	// 3. スタミナ倍率 (3段階: 1.0, 0.8, 0.6)
	static const float staminaTable[] = { 1.0f, 0.8f, 0.6f };
	const float staminamagnification = staminaTable[std::clamp(staminaType, 0, 2)];

	// 4. クリティカル倍率 (最大5段階まで対応可能)
	static const float criticalTable[] = { 1.0f, 1.5f, 2.0f, 2.5f, 3.0f };
	const float criticalmagnification = criticalTable[std::clamp(criLevel, 0, 4)];

	// 5. その他 (AC, SP)
	const float ACmagnification = isAC ? 1.1f : 1.0f;
	const float SPmagnification = isSP ? 1.1f : 1.0f;

	// --- 最終計算 ---
	// baseScore * 全ての倍率
	float totalMag = combomagnification * judgementmagnification * staminamagnification * criticalmagnification * ACmagnification * SPmagnification;

	m_finalScore = static_cast<int>(m_charaBaseScore * totalMag);

	if (m_finalScore > MAX_ONE_NOTE_SCORE) {
		m_finalScore = MAX_ONE_NOTE_SCORE;//上限到達。
	}
	m_totalScore += m_finalScore;

}





RythmGame::~RythmGame() {
	// 生成したNoteオブジェクトを削除
	for (auto* note : m_notes) {
		delete note;
	}
	m_notes.clear();
	DeleteGO(m_perfectSound);
	DeleteGO(m_gameMusic);


}

bool RythmGame::Start() {



	return true;
}

void RythmGame::Init(std::string songName) {
	m_songName = songName;

	

	// 1. JSONロード
	m_chart = m_json->LoadChart(m_songName);

	// 2. サウンド設定
	g_soundEngine->GetWaveFileBank().Release();
	g_soundEngine->ResistWaveFileBank(0, m_chart.songName.c_str());

	m_gameMusic = NewGO<SoundSource>(0, "gameMusic");
	m_gameMusic->Init(0);
	g_soundEngine->ResistWaveFileBank(1, "Assets/SE/shyan.wav");
	m_perfectSound = NewGO<SoundSource>(1, "perfectSound");

	// ★ 判定円の初期化 (右から左仕様：X=-150で固定し、Yをレーンごとに上下に分ける)
	m_judgmentiCrcule.Init("Assets/sprite/parple.DDS", 60.0f, 60.0f);

	m_judgmentiCrcule.SetPosition(Vector3(-150.0f, 0.0f, 0.0f));


	m_judgmentiCrcule.Update();

	// 判定テキストなどの初期化 (変更なし)
	m_currentJudgment = "";
	m_judgmentDisplayTime = 0.0f;
	m_showJudgment = false;
	m_perfectText.Init("Assets/sprite/WONDERFUL.DDS", 500.0f, 180.0f);
	m_greatText.Init("Assets/sprite/Great.DDS", 350.0f, 180.0f);
	m_goodText.Init("Assets/sprite/Nice.DDS", 300.0f, 180.0f);
	m_missText.Init("Assets/sprite/Bad.DDS", 400.0f, 180.0f);
	m_perfectText.SetPosition({ 0.0f, 0.0f, 0.0f });
	m_greatText.SetPosition({ 0.0f, 0.0f, 0.0f });
	m_goodText.SetPosition({ 0.0f, 0.0f, 0.0f });
	m_missText.SetPosition({ 0.0f, 0.0f, 0.0f });

	m_backSprite.Init("Assets/sprite/white.DDS", 1920.0f, 1080.0f);

	// ノーツ生成
	for (auto& noteData : m_chart.notes) {
		Note* note = new Note(noteData);
		note->SetOffset(m_chart.offset);
		m_notes.push_back(note);
	}

	m_gameMusic->Play(false);
	m_musicStartTime = 0.0f;
}

// --- Update 関数
void RythmGame::Update() {
	// 1. サウンドソースの存在チェック（以前の修正を維持）
	auto* sound = FindGO<SoundSource>("gameMusic");
	if (sound == nullptr || m_gameMusic == nullptr) {
		m_isFinished = true;
		m_gameMusic = nullptr;
		return;
	}

	// 2. 音楽の経過時間を取得
	float musicElapsedTime = m_gameMusic->GetPlayTime();
	m_currentTime += g_gameTime->GetFrameDeltaTime();

	// 3. 判定円の更新（Initで設定した X=-150, Y=100/-100 の位置でUpdateされる）
	m_judgmentiCrcule.Update();
	// 7. 全ノーツの更新
// Note::Update(musicElapsedTime) 内で「右から左」への移動計算が行われます
	for (auto& note : m_notes) {
		note->Update(musicElapsedTime);
	}
	// 4. 判定処理（内部でX座標をチェックするように修正したもの）
	Judgment();

	// 5. 判定結果テキスト（PERFECT!など）のアニメーション更新
	if (m_showJudgment) {
		float t = (0.5f - m_judgmentDisplayTime) / 0.5f;
		float scale = 1.0f;
		if (t < 0.2f) {
			scale = 0.5f + (t * 5.0f) * 0.7f;
		}
		else {
			float t2 = (t - 0.2f) / 0.8f;
			scale = 1.2f - (t2 * 0.2f);
		}

		float alpha = 1.0f;
		if (t > 0.7f) {
			float t3 = (t - 0.7f) / 0.3f;
			alpha = 1.0f - t3;
		}

		SpriteRender* targetSprite = nullptr;
		switch (m_currentJudge) {
		case PERFECT: targetSprite = &m_perfectText; break;
		case GREAT:   targetSprite = &m_greatText;   break;
		case GOOD:    targetSprite = &m_goodText;    break;
		case MISS:    targetSprite = &m_missText;    break;
		}

		if (targetSprite) {
			targetSprite->SetScale(Vector3(scale, scale, 1.0f));
			targetSprite->SetMulColor(Vector4(1.0f, 1.0f, 1.0f, alpha));
		}

		m_judgmentDisplayTime -= g_gameTime->GetFrameDeltaTime();
		if (m_judgmentDisplayTime < 0.0f) {
			m_judgmentDisplayTime = 0.0f;
			m_currentJudge = NONE;
			m_showJudgment = false;
		}
	}

	// 6. 背景やデバッグ表示の更新
	m_backSprite.SetMulColor(Vector4(0.0f, 0.0f, 0.0f, 0.5f));
	m_backSprite.Update();



	// 8. コンボ・スコアの更新処理（数値計算）
	CalcMaxCombo();
	if (m_comboCount > m_lastComboCount) {
		m_comboAnimTime = 0.1f;
	}
	m_lastComboCount = m_comboCount;

	if (m_comboAnimTime > 0.0f) {
		m_comboAnimTime -= g_gameTime->GetFrameDeltaTime();
	}

	if (m_displayScore < m_totalScore) {
		int add = (m_totalScore - m_displayScore) / 8 + 100;
		m_displayScore += add;
		if (m_displayScore > m_totalScore) m_displayScore = m_totalScore;
	}

	// 9. 判定済み、または画面外（左端）へ消えたノーツを削除
	m_notes.erase(
		std::remove_if(
			m_notes.begin(),
			m_notes.end(),
			[](Note* n)
			{
				if (!n->IsActive()) {
					delete n;
					return true;
				}
				return false;
			}),
		m_notes.end());
}

// --- PlaySE: SE再生関数 (復活) ---
void RythmGame::PlaySE()
{
	auto se = NewGO<SoundSource>(0, "se");
	se->Init(1); // バンク1のSEを再生
	se->Play(false);
}

// --- Render: 描画処理 (復活・横流れ対応) ---
void RythmGame::Render(RenderContext& rc) {
	m_backSprite.Draw(rc);

	// 判定円の描画
	m_judgmentiCrcule.Draw(rc);

	// 全ノーツの描画
	for (auto& note : m_notes) {
		if (note->IsLongNote()) {
			note->RenderLongNote(rc);
		}
		else {
			note->Render(rc);
		}
	}

	// 判定テキストの描画
	if (m_showJudgment) {
		switch (m_currentJudge) {
		case PERFECT: m_perfectText.Draw(rc); break;
		case GREAT:   m_greatText.Draw(rc);   break;
		case GOOD:    m_goodText.Draw(rc);    break;
		case MISS:    m_missText.Draw(rc);    break;
		}
	}

	// スコア・コンボの描画
	wchar_t scoreStr[512];
	swprintf_s(scoreStr, 512, L"Score: %09d / %09d", m_displayScore, m_targetScore);
	m_totalScoreFont.SetText(scoreStr);
	m_totalScoreFont.SetPosition(Vector3(280.0f, -440.0f, 0.0f));
	m_totalScoreFont.Draw(rc);

	wchar_t comboStr[256];
	swprintf_s(comboStr, 256, L"%d COMBO", m_comboCount);
	m_comboFont.SetText(comboStr);
	m_comboFont.SetPosition(Vector3(420, 420, 0.0));
	m_comboFont.Draw(rc);
}


void RythmGame::Judgment()
{
	bool leftKeyTriggered = g_pad[0]->IsTrigger(enButtonA);
	bool leftKeyHeld = g_pad[0]->IsPress(enButtonA);

	// 自作Release判定
	bool leftKeyReleased = (m_wasLeftKeyHeld && !leftKeyHeld);

	// --- 1. 通常タップノーツの判定 ---
	if (leftKeyTriggered) {
		Note* targetNote = nullptr;
		float minDistance = 999999.0f;

		for (auto& note : m_notes) {
			// まだ判定されておらず、かつタップノーツであること
			if (!note->IsActive() || note->IsJudged() || note->IsLongNote()) continue;

			float noteX = note->GetXPos();
			float distance = fabs(noteX - (-150.0f));

			if (distance <= 50.0f) {
				if (distance < minDistance) {
					minDistance = distance;
					targetNote = note;
				}
			}
		}

		if (targetNote != nullptr) {
			if (minDistance <= 10.0f) { m_currentJudge = PERFECT; m_comboCount++; }
			else if (minDistance <= 25.0f) { m_currentJudge = GREAT; m_comboCount++; }
			else { m_currentJudge = GOOD; m_comboCount = 0; }

			targetNote->SetJudged(true);
			targetNote->SetActive(false);
			m_showJudgment = true;
			m_judgmentDisplayTime = 0.5f;
			PlaySE();
			auto eff = NewGO<HitEffect>(0, "effect");
			eff->Init(Vector3(-150.0f, 0.0f, 0.0f));
		}
	}

	// --- 2. ロングノーツの判定 ---
	for (auto& note : m_notes) {
		if (!note->IsActive() || note->IsJudged() || !note->IsLongNote()) continue;

		// A. 開始判定
		if (!note->IsLongNoteStarted()) {
			// 判定ライン付近で押したか
			if (note->IsLongNoteStartInJudgmentRange() && leftKeyTriggered) {
				note->SetLongNoteStarted(true);
				note->SetLongNoteHeld(true);
				m_currentJudge = PERFECT;
				m_comboCount++;
				m_showJudgment = true;
				m_judgmentDisplayTime = 0.5f;
				PlaySE();
				auto eff = NewGO<HitEffect>(0, "effect");
				eff->Init(Vector3(-150.0f, 0.0f, 0.0f));
			}
			// 【重要】開始位置を大幅に（例：-250.0f）過ぎても押されなかったらMISS
			else if (note->GetXPos() < -250.0f) {
				note->SetJudged(true);
				note->SetActive(false);
				m_currentJudge = MISS;
				m_comboCount = 0;
				m_showJudgment = true;
				m_judgmentDisplayTime = 0.5f;
			}
		}
		// B. 保持・終了判定（すでに開始している場合）
		else if (!note->IsLongNoteCompleted()) {
			float endX = note->GetLongNoteEndPosition().x;
			float distance = fabs(endX - (-150.0f));

			if (leftKeyReleased) {
				// 終点が判定ライン付近なら成功
				if (distance <= 60.0f) {
					m_currentJudge = PERFECT;
					m_comboCount++;
					PlaySE();
				}
				else {
					m_currentJudge = MISS;
					m_comboCount = 0;
				}
				note->CompleteLongNote();
				note->SetJudged(true);
				note->SetActive(false);
				m_showJudgment = true;
				m_judgmentDisplayTime = 0.5f;
			}
			// 押しっぱなしで終点を通り過ぎた場合
			else if (leftKeyHeld && endX < -150.0f) {
				m_currentJudge = PERFECT;
				m_comboCount++;
				note->CompleteLongNote();
				note->SetJudged(true);
				note->SetActive(false);
				m_showJudgment = true;
				m_judgmentDisplayTime = 0.5f;
			}
			// 途中で離した場合
			else if (!leftKeyHeld) {
				note->SetJudged(true);
				note->SetActive(false);
				m_currentJudge = MISS;
				m_comboCount = 0;
				m_showJudgment = true;
				m_judgmentDisplayTime = 0.5f;
			}
		}
	}

	// --- 3. 通常タップのみのMiss判定 ---
	for (auto& note : m_notes) {
		if (!note->IsActive() || note->IsJudged() || note->IsLongNote()) continue;

		// 通常タップが判定ラインを過ぎたら消す
		if (note->GetXPos() < -200.0f) {
			note->SetJudged(true);
			note->SetActive(false);
			m_currentJudge = MISS;
			m_comboCount = 0;
			m_showJudgment = true;
			m_judgmentDisplayTime = 0.5f;
		}
	}

	// 最後にキー状態を保存
	m_wasLeftKeyHeld = leftKeyHeld;
}