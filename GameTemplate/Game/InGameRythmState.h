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
    float m_timer = 0.0f;

    bool m_isClear = false;
    std::vector<SongData> m_songList;//楽曲データを保持。


};

