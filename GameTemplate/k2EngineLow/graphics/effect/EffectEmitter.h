#pragma once
#include "Effect.h"

namespace nsK2EngineLow {
	/// <summary>
	/// エフェクトを発生、再生するクラス。
	/// </summary>
	class EffectEmitter : public IGameObject
	{
	public:
		/// <summary>
		/// コンストラクタ。
		/// </summary>
		EffectEmitter();
		/// <summary>
		/// デストラクタ。
		/// </summary>
		~EffectEmitter();
		/// <summary>
		/// 更新処理。
		/// </summary>
		void Update();
		/// <summary>
		/// エフェクトを読み込む。
		/// </summary>
		/// <param name="number">EffectEngine::GetInstance()->ResistEffectで指定した番号。</param>
		void Init(const int number);
		/// <summary>
		/// エフェクトを再生する。
		/// </summary>
		void Play()
		{
			m_effect.Play();
		}
		/// <summary>
		/// エフェクトを停止する。
		/// </summary>
		void Stop()
		{
			m_effect.Stop();
		}
		/// <summary>
		/// 座標を設定。
		/// </summary>
		/// <param name="position">座標。/param>
		void SetPosition(const Vector3& position)
		{
			m_effect.SetPosition(position);
		}
		/// <summary>
		/// 座標を取得。
		/// </summary>
		/// <returns>座標。</returns>
		const Vector3& GetPosition() const
		{
			return m_effect.GetPosition();
		}
		/// <summary>
		/// スケール(拡大率を設定)。
		/// </summary>
		/// <param name="scale">スケール(拡大率)。</param>
		void SetScale(const Vector3& scale)
		{
			m_effect.SetScale(scale);
		}
		/// <summary>
		/// スケール(拡大率)を取得。
		/// </summary>
		/// <returns>スケール(拡大率)。</returns>
		const Vector3& GetScale() const
		{
			return m_effect.GetScale();
		}
		/// <summary>
		/// 回転を設定。
		/// </summary>
		/// <param name="rotation">回転。</param>
		void SetRotation(const Quaternion& rotation)
		{
			m_effect.SetRotation(rotation);
		}
		/// <summary>
		/// 回転を取得。
		/// </summary>
		/// <returns>回転。</returns>
		const Quaternion& GetRotation() const
		{
			m_effect.GetRotation();
		}
		/// <summary>
		/// 再生中かどうかを取得。
		/// </summary>
		/// <returns>再生中ならtrue。</returns>
		const bool IsPlay() const
		{
			return m_effect.IsPlay();
		}

		/// <summary>
		/// 内部のEffectオブジェクトへのポインタを返す。
		/// PlayerAttack.cpp 側で nullptr チェックを行っているため、存在確認が必要な場合はこのメソッドを使用する。
		/// ※注意: m_effect はメンバ変数なので常に非nullptrを返します。もし内部で外部リソース(Effekseer::Effect 等)の有無を
		/// 判定したい場合は Effect クラス側に該当の取得／判定メソッドを実装してください。
		/// </summary>
		Effect* GetEffect()
		{
			return &m_effect;
		}

		const Effect* GetEffect() const
		{
			return &m_effect;
		}

	private:
		Effect m_effect;					//エフェクト。
	};
}
