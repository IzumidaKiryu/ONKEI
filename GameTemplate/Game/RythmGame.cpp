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
	if (r < 50) criLevel = 1; // 10%で1.5倍、などのシンプルな抽選



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

	// サウンドバンクの登録
	g_soundEngine->GetWaveFileBank().Release();
	g_soundEngine->ResistWaveFileBank(0, m_chart.songName.c_str());
	m_gameMusic = NewGO<SoundSource>(0, "gameMusic");
	m_gameMusic->Init(0);
	g_soundEngine->ResistWaveFileBank(1, "Assets/SE/shyan.wav");
	m_perfectSound = NewGO<SoundSource>(1, "perfectSound");



	//判定円の初期化
	m_judgmentiCrcularLeft.Init("Assets/sprite/parple.DDS", 60.0f, 60.0f);
	m_judgmentiCrcularRight.Init("Assets/sprite/parple.DDS", 60.0f, 60.0f);
	m_judgmentiCrcularLeft.SetPosition(Vector3(-150.0f, -150.0f, 0.0f));
	m_judgmentiCrcularRight.SetPosition(Vector3(150.0f, -150.0f, 0.0f));
	m_judgmentiCrcularLeft.Update();
	m_judgmentiCrcularRight.Update();

	// 判定結果表示用の初期化
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

	// ログファイルを開く
	std::ofstream logFile("debug_log.txt");
	logFile << "=== リズムゲーム初期化開始 ===" << std::endl;

	//GPT
	m_chart = m_json->LoadChart(m_songName);

	// デバッグ用：譜面データの確認
	logFile << "譜面読み込み完了: " << m_chart.songName << ", BPM=" << m_chart.bpm << ", Offset=" << m_chart.offset << std::endl;
	logFile << "ノーツ数: " << m_chart.notes.size() << std::endl;

	//ノーツ生成。
	for (auto& noteData : m_chart.notes) {
		// NewGOを使わずに、直接NoteDataを渡して生成
		Note* note = new Note(noteData);

		// オフセットを設定
		note->SetOffset(m_chart.offset);

		// === リストに追加 ===
		m_notes.push_back(note);
		//logFile << "ノーツ生成: time=" << noteData.time << ", lane=" << noteData.lane << ", type=" << noteData.type << ", offset=" << m_chart.offset << ", アドレス=" << (void*)note << std::endl;
	}

	logFile << "生成されたノーツ数: " << m_notes.size() << std::endl;

	// 全ノーツの状態を確認 //一旦ログを減らす。
	//for (size_t i = 0; i < m_notes.size(); i++) {
	//	logFile << "ノーツ[" << i << "]: アドレス=" << (void*)m_notes[i] << ", time=" << m_notes[i]->GetTime() << ", lane=" << m_notes[i]->GetLane() << std::endl;
	//}

	logFile << "=== 初期化完了 ===" << std::endl;
	logFile.close();

	// 音楽スタート
	m_gameMusic->Play(false);
	m_musicStartTime = 0.0f;

}

void RythmGame::Update() {

	if (m_gameMusic->IsPlaying() == false) {//音楽が終了したら結果画面へ。
	
		m_gameMusic->Release();
		this->Deactivate();//曲終わったあとの遷移の場合は自分を非アクティブにする。

		return;
	}
	m_currentTime += g_gameTime->GetFrameDeltaTime();

	// 音楽の経過時間を計算（音楽開始からの経過時間）
	// SoundSourceのGetPlayTime()を使用して正確な音楽再生時間を取得
	float musicElapsedTime = m_gameMusic->GetPlayTime();

	// 判定円の更新
	m_judgmentiCrcularLeft.Update();
	m_judgmentiCrcularRight.Update();

	// 判定処理
	Judgment();

	// 判定結果表示の更新
	if (m_showJudgment) {
		//WhichAndViewJudge(m_currentJudgment,m_judgmentDisplayTime);
		// --- ★アニメーション計算ここから★ ---

	// 経過時間 t (開始時 0.0 → 終了時 1.0 になるように変換)
	// m_judgmentDisplayTime は 0.5f から 0.0f へ減っていくので、
	// (0.5 - 現在値) / 0.5 で、0.0～1.0の進行度を作ります。
		float t = (0.5f - m_judgmentDisplayTime) / 0.5f;

		// 1. スケールのアニメーション (ボヨヨン表現)
		float scale = 1.0f;
		if (t < 0.2f) {
			// 最初の0.1秒(t=0.2)で、0.5倍から1.2倍まで急拡大！
			// (t * 5.0f) は 0.0～1.0 になる
			scale = 0.5f + (t * 5.0f) * 0.7f; // 0.5 + 0.7 = 1.2倍まで
		}
		else {
			// その後は、1.2倍から1.0倍へゆっくり戻る
			// (t - 0.2f) / 0.8f は、残りの時間で 0.0～1.0 になる
			float t2 = (t - 0.2f) / 0.8f;
			scale = 1.2f - (t2 * 0.2f); // 1.2 - 0.2 = 1.0倍まで
		}

		// 2. 透明度のアニメーション (フェードアウト)
		float alpha = 1.0f;
		if (t > 0.7f) {
			// 最後の0.15秒(t=0.7以降)で、1.0から0.0へフェードアウト
			float t3 = (t - 0.7f) / 0.3f;
			alpha = 1.0f - t3;
		}

		// --- ★計算した値をスプライトに適用★ ---

		// 現在表示すべき判定のスプライトを特定
		SpriteRender* targetSprite = nullptr;
		switch (m_currentJudge) {
		case PERFECT: targetSprite = &m_perfectText; break;
		case GREAT:   targetSprite = &m_greatText;   break;
		case GOOD:    targetSprite = &m_goodText;    break;
		case MISS:    targetSprite = &m_missText;    break;
		}

		// 適用！
		if (targetSprite) {
			targetSprite->SetScale(Vector3(scale, scale, 1.0f));
			targetSprite->SetMulColor(Vector4(1.0f, 1.0f, 1.0f, alpha));
			// UpdateはRenderの直前で呼ばれるので、ここでは呼ばなくてOK
			// (もし動かなかったらここに targetSprite->Update(); を追加)
		}

		// --- ★アニメーション計算ここまで★ ---
		m_judgmentDisplayTime -= g_gameTime->GetFrameDeltaTime();

		if (m_judgmentDisplayTime < 0.0f) {

			m_judgmentDisplayTime = 0.0f;
			m_currentJudge = NONE;
			m_showJudgment = false;
		}
	}

	// デバッグ用：現在時刻の確認（1秒に1回だけ表示）
	static float lastDebugTime = 0.0f;
	if (m_currentTime - lastDebugTime >= 1.0f) {
		printf("現在時刻: %.1f秒, 音楽経過時間: %.1f秒, アクティブなノーツ数: %zu\n",
			m_currentTime, musicElapsedTime, m_notes.size());
		lastDebugTime = m_currentTime;
	}

	for (auto& note : m_notes) {
		note->Update(musicElapsedTime); // 音楽の経過時間を渡す

		// デバッグ用：各ノーツの状態を監視
		static int updateCount = 0;
		if (updateCount < 100) { // 最初の100回だけ表示
			printf("ノーツ更新: アドレス=%p, time=%.1f, lane=%d, isActive=%s\n",
				(void*)note, note->GetTime(), note->GetLane(),
				note->IsActive() ? "true" : "false");
			updateCount++;
		}
	}

	CalcMaxCombo();

	// ★コンボが増えた瞬間にアニメーションを開始
	if (m_comboCount > m_lastComboCount) {
		m_comboAnimTime = 0.1f; // 0.3秒かけてアニメーションさせる
	}
	m_lastComboCount = m_comboCount;

	// タイマーを減らす
	if (m_comboAnimTime > 0.0f) {
		m_comboAnimTime -= g_gameTime->GetFrameDeltaTime();
	}

	// 表示スコアを実際のスコアに近づける
	if (m_displayScore < m_totalScore) {
		// 1フレームで増える量（差が大きいほど速く、最後はゆっくり）
		int add = (m_totalScore - m_displayScore) / 8 + 100;
		m_displayScore += add;

		// 追い越さないようにストッパー
		if (m_displayScore > m_totalScore) m_displayScore = m_totalScore;
	}

	//非アクティブなノーツを削除する。
	m_notes.erase(
		std::remove_if(m_notes.begin(), m_notes.end(),
			[](const Note* n) { return !n->IsActive(); }), m_notes.end());

}
void RythmGame::Render(RenderContext& rc) {







	// 1. コンボ数に応じた「ベースの大きさ」を段階的に設定
	float baseScale = 1.0f;

	if (m_comboCount >= 10) {
		baseScale = 1.5f;        // 10コンボ以降：1.5倍
	}
	else if (m_comboCount >= 5) {
		baseScale = 1.25f;       // 5～9コンボ：少し大きめ（1.25倍）
	}
	else {
		baseScale = 1.0f;        // 1～4コンボ：等倍
	}

	// 2. コンボが増えた瞬間の「跳ねるアニメーション」 (0.1秒)
	float animOffset = 0.0f;
	if (m_comboAnimTime > 0.0f) {
		float t = m_comboAnimTime / 0.1f;
		// 跳ねる演出（サイン波の半分）
		animOffset = sinf(t * 3.1415f) * 0.2f;
	}

	// 3. 最終的なスケール（ベース + アニメーション）
	float finalScale = baseScale + animOffset;

	wchar_t combo[256];
	swprintf_s(combo, 256, L"%3d\nCOMBO", m_comboCount);
	m_comboFont.SetText(combo);
	m_comboFont.SetPosition(Vector3(420, 420, 0.0));
	// ★計算したスケールを適用
	m_comboFont.SetScale(finalScale);

	m_comboFont.SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	m_comboFont.SetShadowParam(true, 2.0f, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	m_comboFont.Draw(rc);
	m_judgmentiCrcularLeft.Draw(rc);
	m_judgmentiCrcularRight.Draw(rc);

	wchar_t score[256];
	swprintf_s(score, 256, L"%d", m_finalScore);
	m_getScoreFont.SetText(score);
	m_getScoreFont.SetPosition(Vector3(-30.0, -100, 0.0));
	m_getScoreFont.SetScale(1.0f);
	m_getScoreFont.SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	m_getScoreFont.SetShadowParam(true, 2.0f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));

	// ノーツの描画
	for (auto& note : m_notes) {
		if (note->IsLongNote()) {
			// ロングノーツの場合は専用の描画メソッドを使用
			note->RenderLongNote(rc);
		}
		else {
			// 通常ノーツの場合は通常の描画メソッドを使用
			note->Render(rc);
		}
	}

	// 判定結果の表示
	switch (m_currentJudge)
	{
	case PERFECT:
		m_perfectText.Draw(rc);
		m_getScoreFont.Draw(rc);
		break;
	case GREAT:
		m_greatText.Draw(rc);
		m_getScoreFont.Draw(rc);
		break;
	case GOOD:
		m_goodText.Draw(rc);
		m_getScoreFont.Draw(rc);
		break;
	case MISS:
		m_missText.Draw(rc);
		break;
	case NONE:
		break;
	default:
		break;
	}





	//// スコア表示
	wchar_t scoreStr[512];
	// %09d で「9桁・0埋め」になります
	swprintf_s(scoreStr, 512, L"%09d / %09d", m_displayScore, m_targetScore);

	m_totalScoreFont.SetText(scoreStr);
	// 画面右下の位置（解像度に合わせて調整してください）
	m_totalScoreFont.SetPosition(Vector3(280.0f, -440.0f, 0.0f));
	m_totalScoreFont.SetScale(0.8); // 目標スコア部分は少し小さめ
	m_totalScoreFont.SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f)); // 白
	m_totalScoreFont.SetShadowParam(true, 0.5f, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	m_totalScoreFont.Draw(rc);
	/// スコア表示ここまで


}



void RythmGame::PlaySE()
{
	auto se = NewGO<SoundSource>(0, "se");
	se->Init(1);
	se->Play(false);
}

void RythmGame::Judgment()
{
	bool leftKeyTriggered = g_pad[0]->IsTrigger(enButtonA); // 押した瞬間
	bool rightKeyTriggered = g_pad[0]->IsTrigger(enButtonB);
	bool leftKeyHeld = g_pad[0]->IsPress(enButtonA);        // 押しっぱなし
	bool rightKeyHeld = g_pad[0]->IsPress(enButtonB);

	int targetLane = -1;
	if (leftKeyTriggered) targetLane = 0;
	if (rightKeyTriggered) targetLane = 1;

	// === 通常ノーツ処理 ===
	if (targetLane != -1) {
		Note* nearestNote = nullptr;
		float minDistance = 999999.0f;

		for (auto& note : m_notes) {
			if (!note->IsActive() || note->IsJudged()) continue;
			if (note->GetLane() != targetLane) continue;

			// 通常ノーツだけ対象にする
			if (note->GetType() != "tap") continue;

			float noteY = 720.0f - note->GetYPos();
			float distance = fabs(noteY - (-150.0f)); // 判定ラインY

			if (distance < minDistance) {
				minDistance = distance;
				nearestNote = note;
			}
		}

		// 見つかった通常ノーツを判定
		if (nearestNote != nullptr && minDistance <= 50.0f) {
			if (minDistance <= 10.0f) {
				PlaySE();
				m_perfectCount++;
				m_comboCount++;
				m_currentJudgment = "PERFECT!";
				m_currentJudge = PERFECT;

				// ★★★ 追加: ヒットエフェクト (PERFECT) ★★★
				auto eff = NewGO<HitEffect>(0, "effect");
				Vector3 pos = (nearestNote->GetLane() == 0) ? Vector3(-150.0f, -150.0f, 0.0f) : Vector3(150.0f, -150.0f, 0.0f);
				eff->Init(pos);
			}
			else if (minDistance <= 25.0f) {
				PlaySE();
				m_greatCount++;
				m_comboCount++;
				m_currentJudgment = "GREAT!";
				m_currentJudge = GREAT;
		
				// ★★★ 追加: ヒットエフェクト (PERFECT) ★★★
				auto eff = NewGO<HitEffect>(0, "effect");
				Vector3 pos = (nearestNote->GetLane() == 0) ? Vector3(-150.0f, -150.0f, 0.0f) : Vector3(150.0f, -150.0f, 0.0f);
				eff->Init(pos);
			}
			else {
				PlaySE();
				m_goodCount++;
				m_comboCount = 0;
				m_currentJudgment = "GOOD!";
				m_currentJudge = GOOD;
			
			}

			nearestNote->SetJudged(true);
			nearestNote->SetActive(false);

			m_showJudgment = true;
			m_judgmentDisplayTime = 0.5f;
		}
	}

	// === ロングノーツ処理 ===
	for (auto& note : m_notes) {
		if (!note->IsActive() || note->IsJudged()) continue;
		if (!note->IsLongNote()) continue;

		// --- 1. Start判定 ---
		if (!note->IsLongNoteStarted()) {
			if (note->IsInJudgmentRange()) {
				if ((note->GetLane() == 0 && leftKeyTriggered) ||
					(note->GetLane() == 1 && rightKeyTriggered))
				{
					note->SetLongNoteStarted(true);
					note->SetLongNoteHeld(true);

					PlaySE();
					m_currentJudgment = "PERFECT!";
					m_currentJudge = PERFECT;
					m_comboCount++;
					
					// ★★★ 追加: ロング開始時にもエフェクト！ ★★★
					auto eff = NewGO<HitEffect>(0, "effect");
					Vector3 pos = (note->GetLane() == 0) ? Vector3(-150.0f, -150.0f, 0.0f) : Vector3(150.0f, -150.0f, 0.0f);
					eff->Init(pos);
					m_showJudgment = true;
					m_judgmentDisplayTime = 0.5f;
				}
			}
			continue; // ← Start前はここで終了
		}

		// --- 2. ボディ（途中経過） ---
		if (note->IsLongNoteStarted() && !note->IsLongNoteCompleted()) {
			bool isHolding = ((note->GetLane() == 0 && leftKeyHeld) ||
				(note->GetLane() == 1 && rightKeyHeld));

			if (!isHolding) {
				// 離したら即MISS
				note->SetJudged(true);
				note->SetActive(false);
				m_currentJudgment = "MISS...";
				m_comboCount = 0;
				m_currentJudge = MISS;
				
				m_showJudgment = true;
				m_judgmentDisplayTime = 0.5f;
				continue;
			}
		}

		// --- 3. End判定 ---
		if (note->IsLongNoteEndInJudgmentRange()) {
			if (note->IsLongNoteHeld()) {
				note->CompleteLongNote();
				note->SetJudged(true);
				note->SetActive(false);

				PlaySE();
				m_currentJudgment = "PERFECT!";
				m_comboCount++;
				m_currentJudge = PERFECT;
				
				// ★★★ 追加: ロング終了時にもエフェクト！ ★★★
				auto eff = NewGO<HitEffect>(0, "effect");
				Vector3 pos = (note->GetLane() == 0) ? Vector3(-150.0f, -150.0f, 0.0f) : Vector3(150.0f, -150.0f, 0.0f);
				eff->Init(pos);
			}
			else {
				note->SetJudged(true);
				note->SetActive(false);

				m_currentJudgment = "MISS...";
				m_comboCount = 0;
				m_currentJudge = MISS;
				
			}

			m_showJudgment = true;
			m_judgmentDisplayTime = 0.5f;
		}
	}

	// === Miss判定（通常のみ） ===
	for (auto& note : m_notes) {
		if (!note->IsActive() || note->IsJudged()) continue;

		// ロングノーツはここでは判定しない（開始・保持・終了で個別処理）
		if (note->IsLongNote()) continue;

		// 通常ノーツのヘッド位置が大きく下へ抜けたらMISS
		float headY = 720.0f - note->GetYPos(); // = スプライトY
		if (headY < -200.0f) {
			m_missCount++;
			m_comboCount = 0;
			m_currentJudgment = "MISS!";
			m_currentJudge = MISS;
		

			m_showJudgment = true;
			m_judgmentDisplayTime = 0.5f;

			note->SetJudged(true);
			note->SetActive(false); // ← これも付ける
		}
	}
}