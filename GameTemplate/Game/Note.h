#pragma once
#include "Chart.h"

class Note :public IGameObject
{
public:
	Note(); // デフォルトコンストラクタを追加
	Note(const NoteData& data);//コンストラクタ
	~Note();

	void SetNoteData(const NoteData& data); // セッターメソッドを修正
	void Update(float currentTime);
	void Render(RenderContext& rc);

	bool IsActive() const { return m_isActive; }

	// デバッグ用のゲッターメソッド
	float GetTime() const { return m_data.time; }
	int GetLane() const { return m_data.lane; }
	const std::string& GetType() const { return m_data.type; }
	float GetYPos() const { return m_yPos; } // Y座標を取得するメソッドを追加
	// X座標を取得するメソッドを追加（必要に応じて）  
	float GetXPos() const { return m_sprite.GetPosition().x; }	// 判定用のメソッド
	bool IsInJudgmentRange() const;
	void SetJudged(bool judged) { m_isJudged = judged; }
public:
	void SetActive(bool isActive) { m_isActive = isActive; }

	bool IsJudged() const { return m_isJudged; }
	float GetJudgmentAccuracy() const { return m_judgmentAccuracy; }

	// ロングノーツ用のメソッド
	bool IsLongNote() const { return m_data.duration > 0.0f; }
	float GetDuration() const { return m_data.duration; }
	float GetEndTime() const { return m_data.time + m_data.duration; }
	bool IsLongNoteStarted() const { return m_isLongNoteStarted; }
public:
	void SetLongNoteStarted(bool started) { m_isLongNoteStarted = started; }
	bool IsLongNoteHeld() const { return m_isLongNoteHeld; }
	bool IsLongNoteCompleted() const { return m_isLongNoteCompleted; }
	void StartLongNote() { m_isLongNoteStarted = true; }
	void SetLongNoteHeld(bool held) { m_isLongNoteHeld = held; }
	void CompleteLongNote() { m_isLongNoteCompleted = true; }

	// ロングノーツの判定用メソッド
	bool IsLongNoteStartInJudgmentRange() const;
	bool IsLongNoteEndInJudgmentRange() const;
	bool IsLongNoteInProgress() const;
	Vector3 GetLongNoteEndPosition() const {
		return m_longNoteEnd.GetPosition();
	}
	// ロングノーツの描画用メソッド
	void RenderLongNote(RenderContext& rc);

private:
	NoteData m_data;//ノーツデータ。
	float m_yPos;//画面状のy座標。
	bool m_isActive;//画面に残っているかどうか。
	bool m_isJudged;//判定済みかどうか
	float m_judgmentAccuracy;//判定精度（判定円からの距離）
	float m_offset;//オフセット値

	// ロングノーツ用の状態
	bool m_isLongNoteStarted;//ロングノーツが開始されたか
	bool m_isLongNoteHeld;//ロングノーツが長押しされているか
	bool m_isLongNoteCompleted;//ロングノーツが完了したか

	SpriteRender m_sprite;//スプライトレンダラー。

	// ロングノーツ用のスプライト（毎回初期化しない）
	SpriteRender m_longNoteStart;
	SpriteRender m_longNoteEnd;
	SpriteRender m_longNoteBody;
	bool m_longNoteSpritesInitialized;

public:
	// X座標を設定するメソッドを追加  
	void SetXPos(float xPos) { m_xPos = xPos; }
	// Y座標を設定するメソッドを追加  
	void SetYPos(float yPos) { m_yPos = yPos; }

	// オフセットを設定するメソッドを追加
	void SetOffset(float offset) { m_offset = offset; }

private:
	float m_xPos; // X座標を保持するメンバー変数を追加

};
