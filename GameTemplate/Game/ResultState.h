#pragma once
#include "GameState.h"
#include "sound/SoundSource.h"

class Game;

// リザルト画面の状態を管理するクラス
class ResultState : public IGameState
{
public:
    enum class ResultType
    {
        enClear,
        enGameOver,
        enNum
    };

    ResultState(ResultType type);
    ~ResultState() = default;

    void Initialize(Game* game) override;
    void Update(Game* game) override;
    void FontFade();
    void Render(RenderContext& rc) override;
    // リザルトを表示する前に、外部からスコアとタイムを叩き込むためのメソッド
    void SetResultData(int score, float time) {
        m_targetScore = (float)score;
        m_targetClearTime = time;
    }
	void ScoreRank(); //スコアに応じてランクを決定する関数

    int m_killCount = 0; //キル数を格納する変数

private:
	Game* m_game = nullptr;              // ゲームクラスへのポインタ
    ResultType m_resultType = ResultType::enNum;

    // スプライト関係
    SpriteRender m_BackGroundSprite;    // 背景
    SpriteRender m_blueBandSprite;      // GV風の青い帯

    // フォント関係
    FontRender m_totalScoreFont;        // スコア用
    FontRender m_clearTimeFont;         // タイム用
	FontRender m_killCountFont;        // キル数用

    FontRender m_ButtonFont; //Prees a buttonの文字を描画するためのFontRender
    Vector4 m_fontColor = { 1.0f, 1.0f, 1.0f, 1.0f }; //文字の色

	// サウンド関係
	SoundSource* m_resultSound; //リザルト画面のBGM
	SoundSource* m_timerSound; //タイマーの効果音
	SoundSource* m_rankSound; //ランク表示の効果音
	SoundSource* m_tapSound; //ボタンを押したときの効果音

    float m_timer = 0.0f; //タイマー
    float m_maxTitleTime = 2.0f; //タイトルの最大時間

    bool m_isFontFade = true; //文字のフェードイン・アウトの切り替えフラグ

    // スコア演出用
    float m_targetScore = 0.0f;         // 目標スコア
    float m_displayScore = 0.0f;        // 表示スコア

    // タイム演出用
    float m_targetClearTime = 0.0f;     // 目標タイム(秒)
    float m_displayClearTime = 0.0f;    // 表示タイム(秒)

    // 演出管理
    bool m_isCounting = true;           // カウント中か

    // 演出の状態を管理する列挙型
    enum class Phase {
        enScoreCount,    // スコア加算中
        enTimeCount,     // タイム加算中
        enRankDisplay,   // ランク表示
        enWaitInput      // 入力待ち
    };

	//ランク表示のための列挙型
	enum Rank {
		enS_Plus,//S+ランク
		enS,//Sランク
		enA,//Aランク
		enNum
	};

	int m_rank = 0; //ランクを格納する変数

    Phase m_currentPhase = Phase::enScoreCount;

    SpriteRender m_rankSprite; // ランク用のスプライト
    bool m_isRankSoundPlayed = false; // ランク表示音を一度だけ鳴らす用
    bool m_isChangeFlag = false;//ステートの変更が行われたかどうか（1回のみに限定するため）
    SpriteRender m_resultSprite;
	SpriteRender m_starSprite;
};