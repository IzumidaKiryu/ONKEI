#pragma once
#include "GameState.h"
struct SongData {
    int id;
    std::string title;
    std::string artist;
    std::string jacketPath;
    std::string selsectPath;
    std::string audioPath;
    std::string jsonPath;
};

//インゲーム中のリズムゲームの状態を管理するクラス
//こいつは他と違って現在のInGameStateにオーバーレイする形で動作する
//こいつ動作中はNomalやBossのInGameStateは一時停止。

class RythmGame;
class Player;
class InGameRythmState:public IGameState
{
public:
    enum class RythmPhase
    {
        CutIn,
        StaffZoom,
        Gameplay,
        End
    };
public:
    void Initialize(Game* game) override;
    void Update(Game* game) override;
    void Render(RenderContext& rc) override;

	void UpdateCutIn();//カットインの更新処理
	void UpdateStaffZoom();//五線譜拡大の更新処理
	void UpdateGameplay();//リズムゲームの更新処理
	void FinishRythm();//リズムゲーム終了処理
	void DrawCutIn(RenderContext& rc);//カットインの描画処理
	void DrawStaffZoom(RenderContext& rc);//五線譜拡大の描画処理
	void DrawGameplay(RenderContext& rc);//リズムゲームの描画処理
    SongData LoadSong(const std::string& filePath);


private:
    Game* m_game = nullptr;
	RythmGame* m_rythmGame = nullptr;
    RythmPhase m_phase = RythmPhase::CutIn;
	Player* m_player = nullptr;
    float m_timer = 0.0f;

    bool m_isClear = false;
    std::vector<SongData> m_songList;//楽曲データを保持。

    SpriteRender m_cutInSprite;   // キャラクター
    SpriteRender m_cutInBg;       // 背景の帯
	SpriteRender m_cutInFont;     // 技名のフォント
    Vector3 m_charaPos;     // キャラの座標
    Vector3 m_bgPos;        // 背景の座標
    float m_cutInAlpha = 0.0f; // 共通の透明度

    SpriteRender m_staffSprite;   // 五線譜（判定ライン込）のスプライト
    Vector3 m_staffPos;
    Vector3 m_staffScale;
    float m_staffAlpha = 0.0f;
    float m_staffRotation = 0.0f; // ★追加：回転角（度数法）

    SpriteRender m_screen_Graw;
	// 画面全体のグローエフェクト用スプライト
	float m_grawAlpha = 0.0f; // グローエフェクトの透明度
};

