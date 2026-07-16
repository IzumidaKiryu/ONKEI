#include "stdafx.h"
#include "InGameRythmState.h"
#include "Game.h"
#include "RythmGame.h"
#include "fstream"
#include "k2EngineLowPreCompile.h"
#include <random>  // これを追加
#include <BarnDamage.h>
#include "Player.h"
//リズムゲームのステート
//順番：カットイン→五線譜拡大→リズムゲーム→終了→通常のゲームに戻る
//「私の歌があなたを導く…！」
SongData InGameRythmState::LoadSong(const std::string& filePath) {
    std::ifstream ifs(filePath);
   nlohmann::json j;
    ifs >> j;

    for (auto& v : j["songs"]) {
        SongData song;
        song.id = v.value("id", 0);
        song.title = v.value("title", "");
        song.artist = v.value("artist", "");
        song.jacketPath = v.value("jacket", "");
        song.selsectPath = v.value("select", "");
        song.audioPath = v.value("audio", "");
        song.jsonPath = v.value("json", "");
        m_songList.push_back(song);
    }

    return m_songList[0];
}

void InGameRythmState::Initialize(Game* game)
{
    m_game = game;
    m_phase = RythmPhase::CutIn;
    m_timer = 0.0f;

    // 1. 楽曲データを全件読み込む
    LoadSong("Assets/Json/songData.json");

    // 2. リストが空でないかチェック
    if (!m_songList.empty()) {
        // 3. 乱数生成器の用意（メルセンヌ・ツイスタ）
        std::random_device rd;
        std::mt19937 gen(rd());
        // 0 から (曲数 - 1) までの範囲でランダムな数を作る
        std::uniform_int_distribution<> dis(0, static_cast<int>((m_songList.size() - 1)));

        int randomIndex = dis(gen);

        // 4. 選ばれた曲を渡して初期化
        m_rythmGame = NewGO<RythmGame>(0, "RythmGame");
        m_rythmGame->Init(m_songList[randomIndex].jsonPath.c_str()); // ここでランダムな1曲を渡す
    }

    // カットイン画像の読み込み（サイズは適宜調整してください）
    m_cutInSprite.Init("Assets/UI/skillPlayer.DDS", 1920.0f, 1080.0f);
    m_cutInBg.Init("Assets/sprite/CutInBg.DDS", 1920.0f, 1080.0f);
    m_cutInFont.Init("Assets/UI/skillActionFont.DDS", 1920.0f, 1080.0f);
	m_cutInFont.SetPosition({ 0.0f,0.0f,0.0f });


    // --- 初期位置の設定 ---
    // キャラクター：画面の左端外 (X = -1200.0f くらい)
    m_charaPos = { -1200.0f, 0.0f, 0.0f };
    // 背景：画面の右端外 (X = 1200.0f くらい)
    m_bgPos = { 1200.0f, 0.0f, 0.0f };

    m_cutInAlpha = 0.0f;

	// スタートテキストの初期化
    m_staffSprite.Init("Assets/UI/gameStartFont.DDS", 1920.0f, 1080.0f);//変える
    m_staffPos = { 960.0f, 540.0f, 0.0f }; // 画面中央
    m_staffScale = { 0.1f, 0.1f, 1.0f }; // 最初は小さく
	m_staffAlpha = 0.0f; // 最初は透明

    // 五線譜の初期化
    m_screen_Graw.Init("Assets/sprite/Screen_Graw.DDS", 1920.0f, 1080.0f,AlphaBlendMode_Add);
    m_screen_Graw.SetPosition({ 0, 0, 0 });// 画面中央に置く（全体を覆う）

	//ボタンUIの初期化
	m_buttonSprite.Init("Assets/UI/buttonA.DDS", 1920.0f, 1080.0f);
	m_buttonSprite.SetPosition({ 80.0f, -250.0f, 0.0f });
    m_buttonSprite.SetScale({ 0.2f, 0.2f, 1.0f });
	m_buttonSprite.Update();

    //タップ説明用のテキスト
    wchar_t taskDescription[256];
    std::wstring hff = L"タイミングよく　　 を押そう！";
    m_font.SetText(hff.c_str());
    m_font.SetPosition({-280.0f,-250.0f,0.0f});
    m_font.SetScale(1.0f);
    m_font.SetColor({1.0f,1.0f,1.0f,1.0f});

	// フィニッシュテキストの初期化
	m_finishSprite.Init("Assets/UI/gameFinishFont.DDS", 1920.0f, 1080.0f);
	m_finishPos = { 960.0f, 540.0f, 0.0f }; // 画面中央
	m_finishScale = { 0.1f, 0.1f, 1.0f }; // 最初は小さく
	m_finishAlpha = 0.0f; // 最初は透明

	m_player = FindGO<Player>("player");
}

void InGameRythmState::Update(Game* game)
{
    m_timer += g_gameTime->GetFrameDeltaTime(); // デルタタイム取得関数

    switch (m_phase)
    {
    case RythmPhase::CutIn:
        UpdateCutIn();
        break;
    case RythmPhase::StaffZoom:
        UpdateStaffZoom();
        break;
    case RythmPhase::Gameplay:
        UpdateGameplay();
        // ★ ここでチェック！
        // RythmGameが終了（Deactivate）していたら、Endフェーズへ移行させる
        if (m_rythmGame != nullptr && m_rythmGame->IsFinished()) {
            // 1. スコアとコンボを取得
            int totalScore = m_rythmGame->GetTotalScore(); // RythmGame.hにGetterが必要
            int maxCombo = m_rythmGame->GetCombo();     // RythmGame.hにGetterが必要

            // ★リズムゲームで稼いだスコアを合計スコアに加算する。
            //   （リザルトの合計スコア＝雑魚敵の撃破スコア＋ここで貯めたスコア）
            m_game->m_rythmScore += totalScore;

            // 2. ダメージ量の計算アルゴリズム（例：スコアの 1/100 + コンボボーナス）
            float finalDamage = (totalScore * 0.01f) + (1.0f + (maxCombo * 0.05f));
            float range = 1000.0f; // 攻撃範囲

            // 3. バーンダメージオブジェクトの生成
            // プレイヤーの座標（Gameクラス等から取得）を中心に発動
            Vector3 playerPos = m_player->GetPosition();
            auto barn = NewGO<BarnDamage>(0, "BarnDamage");
            barn->Init(finalDamage, range, playerPos);
			DeleteGO(m_rythmGame); // 終了したリズムゲームオブジェクトを削除
            m_phase = RythmPhase::End;
        }
        break;
    case RythmPhase::End:
        FinishRythm();
        break;
    }
}

void InGameRythmState::Render(RenderContext& rc)
{
    // フェーズごとの描画
    switch (m_phase)
    {
    case RythmPhase::CutIn:
        DrawCutIn(rc);
        break;
    case RythmPhase::StaffZoom:
        DrawStaffZoom(rc);
        break;
    case RythmPhase::Gameplay:
        DrawGameplay(rc);
        break;
    case RythmPhase::End:
		DrawFinish(rc);
        break;
    }
}



void InGameRythmState::UpdateCutIn()
{
    float deltaTime = g_gameTime->GetFrameDeltaTime();

    if (m_timer < 0.8f) {
        // --- 登場フェーズ（最初の0.8秒） ---
        // キャラクターを右へ移動
        m_charaPos.x += 2500.0f * deltaTime;
        if (m_charaPos.x > 0.0f) m_charaPos.x = 0.0f; // 中央で止める

        // 背景を左へ移動
        m_bgPos.x -= 2500.0f * deltaTime;
        if (m_bgPos.x < 0.0f) m_bgPos.x = 0.0f; // 中央で止める

        // フェードイン
        m_cutInAlpha += 2.0f * deltaTime;
    }
    else if (m_timer < 2.2f) {
        // --- 溜めフェーズ（0.8~2.2秒） ---
        // 中央付近でそれぞれ超ゆっくり動かす（お好みで）
        m_charaPos.x += 10.0f * deltaTime;
        m_bgPos.x -= 10.0f * deltaTime;
    }
    else {
        // --- 退場フェーズ（最後の0.8秒） ---
        // キャラクターをさらに右へ加速
        m_charaPos.x += 3000.0f * deltaTime;
        // 背景をさらに左へ加速
        m_bgPos.x -= 3000.0f * deltaTime;

        // フェードアウト
        m_cutInAlpha -= 2.0f * deltaTime;
    }

    // 透明度の制限
    if (m_cutInAlpha > 1.0f) m_cutInAlpha = 1.0f;
    if (m_cutInAlpha < 0.0f) m_cutInAlpha = 0.0f;

    // スプライトに反映
    m_cutInSprite.SetPosition(m_charaPos);
    m_cutInBg.SetPosition(m_bgPos);

    // エンジンの機能に応じて透明度を設定してください
     m_cutInSprite.SetMulColor({1, 1, 1, m_cutInAlpha});
     m_cutInBg.SetMulColor({1, 1, 1, m_cutInAlpha});

    m_cutInSprite.Update();
    m_cutInBg.Update();
    // --- モヤ(m_screen_Graw)の激しい明滅処理 ---

    // 0.0 ～ 1.0 のランダムな値を作る
    float randomFlicker = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

    // アルファ値だけでなく、RGBの倍率として randomFlicker を使う
    // 0.3 ～ 1.0 の間で色が強烈に変化します
    float power = (0.3f + randomFlicker * 0.7f) * m_cutInAlpha;

    // 全ての成分に power を掛ける
    m_screen_Graw.SetMulColor({ 0.2f * power, 0.6f * power, 1.0f * power, m_cutInAlpha });
    m_screen_Graw.Update();

    // 次のフェーズへ
    if (m_timer > 3.0f)
    {
        m_phase = RythmPhase::StaffZoom;
        m_timer = 0.0f;
    }
}

void InGameRythmState::UpdateStaffZoom()
{

    //また今度やろう、ちょっと変
    float deltaTime = g_gameTime->GetFrameDeltaTime();

    // 0.0 ～ 0.8 の時間を 0.0 ～ 1.0 の割合（t）に変換
    float t = m_timer / 0.8f;
    if (t > 1.0f) t = 1.0f;

    // --- イージング処理 (Ease Out Expo的な動き) ---
    // tが1に近づくほど、増え方がゆっくりになる
    float easeOut = 1.0f - pow(2.0f, -10.0f * t);

    // スケール：0.1 から 1.0 へ
    float currentScale = 0.1f + (0.9f * easeOut);
    m_staffScale = { currentScale, currentScale, 1.0f };

    // 透明度：0.0 から 1.0 へ
    m_staffAlpha = easeOut;

    // 座標：奥（中心）から、本来の表示位置（例：Y=0）へ
    // 遠近感を出すために、少し上から降りてくるようにするとGOOD
    m_staffPos.x = 0.0f;
    m_staffPos.y = 200.0f * (1.0f - easeOut); // 200から0へ降りてくる
    m_staffPos.z = 0.0f;

    // スプライトに反映
    m_staffSprite.SetPosition(m_staffPos);
    m_staffSprite.SetScale(m_staffScale);
    m_staffSprite.SetMulColor({1, 1, 1, m_staffAlpha});
    m_staffSprite.Update();

    if (m_timer > 0.8f)
    {
        m_phase = RythmPhase::Gameplay;
        m_timer = 0.0f;
        if (m_rythmGame) m_rythmGame->GamePlay(); // ここで音楽ドン！
    }
}

void InGameRythmState::UpdateGameplay()
{
    // TODO: ノーツ判定・判定UI更新
}

void InGameRythmState::FinishRythm()
{
    //また今度やろう、ちょっと変
    float deltaTime = g_gameTime->GetFrameDeltaTime();

    // 0.0 ～ 0.8 の時間を 0.0 ～ 1.0 の割合（t）に変換
    float t = m_timer / 0.8f;
    if (t > 1.0f) t = 1.0f;

    // --- イージング処理 (Ease Out Expo的な動き) ---
    // tが1に近づくほど、増え方がゆっくりになる
    float easeOut = 1.0f - pow(2.0f, -10.0f * t);

    // スケール：0.1 から 1.0 へ
    float currentScale = 0.1f + (0.9f * easeOut);
    m_finishScale = { currentScale, currentScale, 1.0f };

    // 透明度：0.0 から 1.0 へ
    m_finishAlpha = easeOut;

    // 座標：奥（中心）から、本来の表示位置（例：Y=0）へ
    // 遠近感を出すために、少し上から降りてくるようにするとGOOD
    m_finishPos.x = 0.0f;
    m_finishPos.y = 200.0f * (1.0f - easeOut); // 200から0へ降りてくる
    m_finishPos.z = 0.0f;

    // スプライトに反映
    m_finishSprite.SetPosition(m_finishPos);
    m_finishSprite.SetScale(m_finishScale);
    m_finishSprite.SetMulColor({ 1, 1, 1, m_finishAlpha });
    m_finishSprite.Update();

    if (m_timer > 2.0f)
    {
        m_timer = 0.0f;
        // InGameNormalStateに戻す or ResultStateへ
        m_game->PopState(); // リズムゲーム終了後、通常のゲームに戻る
    }
        

 
}

void InGameRythmState::DrawCutIn(RenderContext& rc)
{
	
    // カットインスプライト描画
    // 先に背景（帯）を描画
    m_cutInBg.Draw(rc);
    // その上にキャラクターを描画
    m_cutInSprite.Draw(rc);
	// 最後に技名のフォントを描画
    m_cutInFont.Draw(rc);
    m_screen_Graw.Draw(rc); // ここに入れると背景が光る

}

void InGameRythmState::DrawStaffZoom(RenderContext& rc)
{
    // 五線譜拡大アニメ描画
    m_staffSprite.Draw(rc);
   
}

void InGameRythmState::DrawGameplay(RenderContext& rc)
{
    // ノーツ・判定UI描画
     //ボタンUIの描画
    m_buttonSprite.Draw(rc);
    //説明用テキストの描画
    m_font.Draw(rc);
    
	// リズムゲームの描画を呼び出す
	//m_rythmGame->Render(rc);
}

void InGameRythmState::DrawFinish(RenderContext& rc)
{
	// 終了エフェクト描画
	m_finishSprite.Draw(rc);
}
