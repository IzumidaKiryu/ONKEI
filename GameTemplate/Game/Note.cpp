#include "stdafx.h"
#include "Note.h"

Note::Note()
    : m_yPos(0.0f), m_isActive(false), m_isJudged(false), m_judgmentAccuracy(0.0f),
    m_isLongNoteStarted(false), m_isLongNoteHeld(false), m_isLongNoteCompleted(false),
    m_longNoteSpritesInitialized(false), m_offset(0.0f) // ロングノーツの状態も初期化
{
    // デフォルトコンストラクタでは何も初期化しない
    // スプライトは初期化しない（SetNoteDataで後から初期化される）
    // m_isActive = false なので描画されない
}

Note::Note(const NoteData& data)
    :m_data(data), m_yPos(0.0f), m_isActive(true), m_isJudged(false), m_judgmentAccuracy(0.0f),
    m_isLongNoteStarted(false), m_isLongNoteHeld(false), m_isLongNoteCompleted(false),
    m_longNoteSpritesInitialized(false), m_offset(0.0f)
{

    m_sprite.Init("Assets/UI/tapNote.DDS", 500.0f,250.0f);

    // ロングノーツ用のスプライトを初期化
    if (IsLongNote()) {
        m_longNoteStart.Init("Assets/UI/note.DDS", 500.0f, 250.0f);
        m_longNoteEnd.Init("Assets/UI/note.DDS", 500.0f, 250.0f);
        m_longNoteBody.Init("Assets/sprite/white.DDS", 60.0f, 60.0f);
        m_longNoteBody.SetMulColor({ 0.8f,1.0f,1.0f,1.0f });//淡い水色
        m_longNoteSpritesInitialized = true;
    }

    // 初期座標を正しく設定
    float xPos = 720.0f;   // 右端から出現
    Vector3 initialPos = { xPos, 0.0f, 0.0f };
    m_sprite.SetPosition(initialPos);
    m_longNoteStart.SetPosition(initialPos);
    m_longNoteEnd.SetPosition(initialPos);
    m_longNoteBody.SetPosition(initialPos);

    // スプライトの更新を即座に反映
    m_sprite.Update();
    m_longNoteStart.Update();
    m_longNoteEnd.Update();
    m_longNoteBody.Update();
}

Note::~Note() = default;

void Note::SetNoteData(const NoteData& data)
{
    //m_data = data;
    //m_yPos = 0.0f;
    //m_isActive = true;
    //m_isJudged = false;
    //m_judgmentAccuracy = 0.0f;
    //m_isLongNoteStarted = false;
    //m_isLongNoteHeld = false;
    //m_isLongNoteCompleted = false;
    //m_longNoteSpritesInitialized = false;
    //m_sprite.Init("Assets/sprite/Nozomi.DDS", 60.0f, 60.0f);

    //// ロングノーツ用のスプライトを初期化
    //if (IsLongNote()) {
    //    m_longNoteStart.Init("Assets/sprite/Nozomi.DDS", 60.0f, 60.0f);
    //    m_longNoteEnd.Init("Assets/sprite/Nozomi.DDS", 60.0f, 60.0f);
    //    m_longNoteBody.Init("Assets/sprite/yattabe.DDS", 60.0f, 60.0f);
    //    m_longNoteSpritesInitialized = true;
    //}

    //// 初期座標を正しく設定
    //// レーン0を左側(-150,0,0)、レーン1を右側(150,0,0)に配置
    //float xPos = (data.lane == 0) ? -150.0f : 150.0f;
    //Vector3 initialPos = { xPos, 720.0f, 0.0f };
    //m_sprite.SetPosition(initialPos);

    //// スプライトの更新を即座に反映
    //m_sprite.Update();
}

void Note::Update(float currentTime)
{
    float speed = 300.0f;
    float targetTime = m_data.time + m_offset; // 「この時間」に判定ラインにいたい
    const float lineX = -150.0f;              // 判定ラインの座標

    // 1. 本来あるべき物理的な X 座標を計算 (逆算式)
    // (叩くべき時間 - 現在の時間) * スピード = 判定ラインからの距離
    float secondsUntilHit = targetTime - currentTime;
    Vector3 pos;
    pos.x = lineX + (secondsUntilHit * speed);
    pos.y = 0.0f;
    pos.z = 0.0f;

    // --- 描画制御 (ここから下は理想の動きのまま) ---
    Vector3 visualStartPos = pos;

    // 画面の右端（例えば 960.0f）より外側なら、描画座標を固定して隠しておく
    // これで「パッと現れる」のを防げます
    bool isVisible = (pos.x < 960.0f);

    // ロングノーツ保持中のみラインで止める
    if (IsLongNote() && m_isLongNoteHeld) {
        if (visualStartPos.x < lineX) {
            visualStartPos.x = lineX;
        }
    }

    m_sprite.SetPosition(visualStartPos);

    // ロングノーツの各パーツ更新
    if (IsLongNote() && m_longNoteSpritesInitialized) {
        float longWidth = m_data.duration * speed;
        Vector3 endPos = pos;
        endPos.x += longWidth;
        m_longNoteEnd.SetPosition(endPos);
        m_longNoteStart.SetPosition(visualStartPos);

        float visibleLeft = visualStartPos.x;
        float visibleRight = endPos.x;
        float visibleWidth = visibleRight - visibleLeft;

        if (visibleWidth > 0) {
            Vector3 bodyPos;
            bodyPos.x = (visibleLeft + visibleRight) * 0.5f;
            m_longNoteBody.SetPosition(bodyPos);
            m_longNoteBody.SetScale({ visibleWidth / 60.0f, 1.0f, 1.0f });
        }
        else {
            m_longNoteBody.SetScale({ 0.0f, 0.0f, 0.0f });
        }
    }

    m_sprite.Update();

    // 消去判定
    float deleteThreshold = -400.0f;
    if (IsLongNote()) {
        float endX = m_longNoteEnd.GetPosition().x;
        if (endX < deleteThreshold && !m_isLongNoteHeld) {
            m_isActive = false;
        }
    }
    else {
        float noteX = m_sprite.GetPosition().x;
        if (noteX < deleteThreshold) {
            m_isActive = false;
        }
    }

    if (IsLongNote()) {
        m_longNoteStart.Update();
        m_longNoteEnd.Update();
        m_longNoteBody.Update();
    }
}
void Note::Render(RenderContext& rc) {
    if (!m_isActive) return;

    // デフォルトコンストラクタで生成されたオブジェクトは描画しない
    // m_data.timeが0の場合は初期化されていないと判断
    if (m_data.time == 0.0f) return;

    // デバッグ用：描画されるノーツの情報を表示
    static int renderCount = 0;
    if (renderCount < 20) { // 最初の20回だけ表示
        Vector3 currentPos = m_sprite.GetPosition();
        printf("ノーツ描画: lane=%d, 現在座標(%.1f, %.1f, %.1f), yPos=%.1f, isActive=%s\n",
            m_data.lane, currentPos.x, currentPos.y, currentPos.z, m_yPos, m_isActive ? "true" : "false");
        renderCount++;
    }

    // 位置設定はUpdate内で行うため、ここでは描画のみ
    m_sprite.Draw(rc);
}

bool Note::IsInJudgmentRange() const
{
    const float JUDGE_LINE_X = -150.0f;
    const float RANGE = 50.0f;

    float noteX = m_sprite.GetPosition().x;

    float distance = fabs(noteX - JUDGE_LINE_X);

    return distance <= RANGE;
}

// ロングノーツが進行中かどうか（開始済みで完了していない）
bool Note::IsLongNoteInProgress() const
{
    return IsLongNote() && m_isLongNoteStarted && !m_isLongNoteCompleted;
}

// ロングノーツの描画（長さを視覚的に表現）
void Note::RenderLongNote(RenderContext& rc)
{

    if (!IsLongNote() || !m_longNoteSpritesInitialized) return;

    // Updateで位置とスケールを更新済みなので描画だけ
    m_longNoteBody.Draw(rc);
    m_longNoteStart.Draw(rc);
    m_longNoteEnd.Draw(rc);
}

// ロングノーツ開始が判定ラインにあるか？
bool Note::IsLongNoteStartInJudgmentRange() const
{
    if (!IsLongNote()) return false;

    float judgmentX = -150.0f;
    float range = 50.0f;         // 判定範囲

    float distance = fabs(m_sprite.GetPosition().x - judgmentX);
    return distance <= range;
}

// ロングノーツ終了が判定ラインにあるか？
bool Note::IsLongNoteEndInJudgmentRange() const
{
    if (!IsLongNote()) return false;

    float judgmentX = -150.0f;
    float range = 50.0f;

    Vector3 endPos = m_longNoteEnd.GetPosition();
    float distance = fabs(endPos.x - judgmentX);

    return distance <= range;
}