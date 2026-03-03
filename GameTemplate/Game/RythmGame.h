#pragma once
#include "Chart.h"
#include "sound/SoundSource.h"

enum EnJudge
{
	PERFECT,
	GREAT,
	GOOD,
	MISS,
	NONE
};

class Note;
class ReadJSON;
class RythmGame :public IGameObject
{
public:
	RythmGame();
	~RythmGame();
	bool Start();
	void Init(std::string songName);
	void Update();
	void Judgment(); // 判定処理を追加
	void Render(RenderContext& rc);
	void WhichAndViewJudge(std::string judge, float showTime); // 判定結果に応じた処理
	void CalcMaxCombo();// 最大コンボ数の計算
	/// <summary>
	///	スコア計算と加算処理
	/// </summary>
	/// <param name="charaBaseScore">キャラごとのアピール値</param>
	/// <param name="comboCount">コンボ数</param>
	/// <param name="judgeType">判定</param>
	/// <param name="staminaType">スタミナゲージの段階</param>
	/// <param name="criLevel">クリティカルか否か</param>
	/// <param name="isAC">アピールチャンス中か否か</param>
	/// <param name="isSP">スペシャル発動後３秒いないか否か</param>
	void CalcScore(/*int charaBaseScore,*/ /*int comboCount,*/ int judgeType, int staminaType, /*int criLevel,*/ bool isAC, bool isSP);

private:
	SpriteRender m_judgmentiCrcularLeft;
	SpriteRender m_judgmentiCrcularRight;
	// 判定結果表示用
	SpriteRender m_judgmentText;
	std::string m_currentJudgment; // 現在の判定結果
	float m_judgmentDisplayTime = 0.0f;   // 判定結果の表示時間
	bool m_showJudgment = false;           // 判定結果を表示するかどうか
	ReadJSON* m_json = nullptr;
	float m_comboAnimTime = 0.0f; // コンボアニメーションの経過時間
	int m_lastComboCount = 0;    // 前フレームのコンボ数を覚えておく用
	SoundSource* m_gameMusic = nullptr;
	SoundSource* m_perfectSound = nullptr;
	SoundSource* m_SPSound = nullptr;
	void PlaySE();

private:
	ChartData m_chart; // チャートデータ
	std::vector<Note*> m_notes; // ノーツオブジェクトのリスト

	float m_currentTime = 0.0f;
	float m_musicStartTime = 0.0f; // 音楽の開始時刻

	int m_totalScore = 0;  // このライブ中に獲得した総スコア
	int m_displayScore = 0;    // 画面表示用のスコア（徐々に増える）
	int m_targetScore = 7122000; // 目標スコア(ToDo.あとで曲ごとに変更できるようにしてね)
	int m_charaBaseScore = 100;//基礎スコア

	// 判定関連
	int m_perfectCount = 0;
	int m_greatCount = 0;
	int m_goodCount = 0;
	int m_missCount = 0;
	int m_comboCount = 0;
	int m_maxCombo = 0;
	int m_finalScore = 0;//１ノーツあたりの最終スコア

	// ロングノーツ用の状態
	bool m_leftLaneHeld = false;  // 左レーンが長押しされているか
	bool m_rightLaneHeld = false; // 右レーンが長押しされているか
	//判定表示用スプライト。
	SpriteRender m_perfectText;
	SpriteRender m_greatText;
	SpriteRender m_goodText;
	SpriteRender m_missText;

	FontRender m_comboFont;
	FontRender m_getScoreFont;
	FontRender m_totalScoreFont;
	EnJudge m_currentJudge = NONE;

	bool m_isModelLoading = true;
	// ロード管理用
	std::thread m_loadingThread;       // ファイル読み込み用スレッド
	std::atomic<bool> m_isDiskLoadFinished{ false }; // ディスク読み込みが終わったか？
	bool m_isGPUInitFinished{ false }; // GPU初期化（テクスチャ等）が終わったか？

public:
	// Loading画面から「終わった？」と聞くための関数
	bool IsReady() const { return m_isGPUInitFinished; }

private:
	std::string m_songName;
};


