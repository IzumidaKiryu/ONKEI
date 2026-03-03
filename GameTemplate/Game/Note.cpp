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
    m_sprite.Init("Assets/sprite/TapNote.DDS", 60.0f, 60.0f);

    // ロングノーツ用のスプライトを初期化
    if (IsLongNote()) {
        m_longNoteStart.Init("Assets/sprite/LongNote.DDS", 60.0f, 60.0f);
        m_longNoteEnd.Init("Assets/sprite/LongNote.DDS", 60.0f, 60.0f);
        m_longNoteBody.Init("Assets/sprite/white.DDS", 60.0f, 60.0f);
        m_longNoteSpritesInitialized = true;
    }

    // 初期座標を正しく設定
    // レーン0を左側(-150,0,0)、レーン1を右側(150,0,0)に配置
    float xPos = (data.lane == 0) ? -150.0f : 150.0f;
    Vector3 initialPos = { xPos, 720.0f, 0.0f };
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
    m_data = data;
    m_yPos = 0.0f;
    m_isActive = true;
    m_isJudged = false;
    m_judgmentAccuracy = 0.0f;
    m_isLongNoteStarted = false;
    m_isLongNoteHeld = false;
    m_isLongNoteCompleted = false;
    m_longNoteSpritesInitialized = false;
    m_sprite.Init("Assets/sprite/Nozomi.DDS", 60.0f, 60.0f);

    // ロングノーツ用のスプライトを初期化
    if (IsLongNote()) {
        m_longNoteStart.Init("Assets/sprite/Nozomi.DDS", 60.0f, 60.0f);
        m_longNoteEnd.Init("Assets/sprite/Nozomi.DDS", 60.0f, 60.0f);
        m_longNoteBody.Init("Assets/sprite/yattabe.DDS", 60.0f, 60.0f);
        m_longNoteSpritesInitialized = true;
    }

    // 初期座標を正しく設定
    // レーン0を左側(-150,0,0)、レーン1を右側(150,0,0)に配置
    float xPos = (data.lane == 0) ? -150.0f : 150.0f;
    Vector3 initialPos = { xPos, 720.0f, 0.0f };
    m_sprite.SetPosition(initialPos);

    // スプライトの更新を即座に反映
    m_sprite.Update();
}

void Note::Update(float currentTime)
{
    // デフォルトコンストラクタで生成されたオブジェクトは処理しない
    if (m_data.time == 0.0f) return;

    float speed = 300.0f;
    float spawnTime = m_data.time + m_offset; // オフセットを適用

    if (currentTime >= spawnTime) {
        m_yPos = (currentTime - spawnTime) * speed;

        float xPos = (m_data.lane == 0) ? -150.0f : 150.0f;
        Vector3 startPos = { xPos, 720.0f - m_yPos, 0.0f };
        m_sprite.SetPosition(startPos);

        // ロングノーツの座標もここで更新！
        if (IsLongNote() && m_longNoteSpritesInitialized) {
            float longNoteHeight = m_data.duration * speed;

            Vector3 endPos = startPos;
            endPos.y += longNoteHeight;

            m_longNoteStart.SetPosition(startPos);
            m_longNoteEnd.SetPosition(endPos);

            // 本体は真ん中に配置
            Vector3 bodyPos = startPos;
            bodyPos.y += longNoteHeight * 0.5f;
            m_longNoteBody.SetPosition(bodyPos);

            // 本体の高さを duration に合わせて変更
            m_longNoteBody.SetScale({ 1.0f, longNoteHeight / 60.0f, 1.0f });
        }
    }

    if (m_data.type == "tap") {
        if (m_yPos > 1020.0f) {
            m_isActive = false;
        }
    }
    else if (m_data.type == "long") {
        if (m_longNoteEnd.GetPosition().y < -270.0f) // ロングノーツの終端が画面外に出たら非アクティブ。
        {
            m_isActive = false;
        }
    }

    m_sprite.Update();
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
    // 判定円の位置（y = -150.0f）の周辺で判定
    float judgmentY = -150.0f;
    float judgmentRange = 50.0f; // 判定範囲（ピクセル）

    Vector3 currentPos = m_sprite.GetPosition();
    float distance = abs(currentPos.y - judgmentY);

    return distance <= judgmentRange;
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

    float judgmentY = -150.0f;   // 判定円のY座標
    float range = 50.0f;         // 判定範囲

    float distance = fabs(m_sprite.GetPosition().y - judgmentY);
    return distance <= range;
}

// ロングノーツ終了が判定ラインにあるか？
bool Note::IsLongNoteEndInJudgmentRange() const
{
    if (!IsLongNote()) return false;

    float judgmentY = -150.0f;
    float range = 50.0f;

    // endのスプライトを基準に判定する
    Vector3 endPos = m_longNoteEnd.GetPosition();
    float distance = fabs(endPos.y - judgmentY);
    return distance <= range;
}