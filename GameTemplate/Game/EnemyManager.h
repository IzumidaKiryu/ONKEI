#pragma once
#include <vector> // vectorを使えるようにする
#include "Enemy.h"

class Enemy;
class Boss;

// 敵の初期配置用データ構造体
struct EnemySpawnData {
    Vector3 pos;
};

class EnemyManager : public IGameObject {
public:
	//自分が生成した敵を必ず道連れにする。
	//これが無いと、EnemyManagerだけがDeleteGOされて敵が野良のまま生き残り、
	//リザルト画面にHPゲージだけが残ってしまう。
	~EnemyManager();
    void Init();
    void Update() override;

	//雑魚敵1体あたりの撃破スコア
	static constexpr int KILL_SCORE_PER_ENEMY = 1000;

	//チェインが途切れるまでの猶予（秒）。この間に次を倒せばチェインが継続する。
	static constexpr float CHAIN_WINDOW_TIME = 3.0f;
	//チェイン倍率の上限。
	static constexpr float CHAIN_MULTIPLIER_MAX = 3.0f;
	//1チェインあたりの倍率の増分。
	static constexpr float CHAIN_MULTIPLIER_STEP = 0.1f;

	//撃破数と撃破スコアを加算する。ラッシュ中とチェイン中はスコアが増える。
    void CountUpDeathCount();
	//現在のチェイン数によるスコア倍率を返す。
    float GetChainMultiplier() const;
	//現在の連続撃破数。
    int GetChainCount() const { return m_chainCount; }
    int GetDeathCount() const { return m_deathCount; }
	//撃破で稼いだスコアの合計を返す
    int GetKillScore() const { return m_killScore; }
	//ラッシュ（残り30秒以降）の切り替え。フィールド上の敵の最大数と湧く速さが倍になる。
    void SetRushMode(bool isRush) { m_isRushMode = isRush; }
    bool IsRushMode() const { return m_isRushMode; }
	//スポーンを打ち切る。ゲーム終了時に呼び、演出中に敵が湧かないようにする。
    void StopSpawn() { m_isSpawnActive = false; }
    void ClearAllEnemies(); // 全削除用関数
    // 敵が死んだときに呼び出してもらう関数
    void OnEnemyDestroy(Enemy* enemy);
    void SpawnEnemies(); // 敵を生成する関数
    void SpawnOneEnemy(); // 敵を1体だけ生成する関数
	void BossSpawn(); // ボスをスポーンさせる関数
	void SetBossDeathFlag(bool isDead) { m_BossDeathFlag = isDead; }
	bool GetBossDeathFlag() { return m_BossDeathFlag; }

    // 管理している敵のリストを返すゲッターを追加
    const std::vector<Enemy*>& GetEnemyList() const {
        return m_enemies;
    }

    Boss* GetBoss() const { return m_boss; }

    int m_deathCount = 0;
    int m_killScore = 0;//撃破スコアの合計
    int m_chainCount = 0;//連続撃破数。一定時間倒さないと0に戻る。
    float m_chainTimer = 0.0f;//チェインが切れるまでの残り時間

private:

    Boss* m_boss = nullptr; // ボスのポインタ
    // 配置データをリスト化して持つ
    std::vector<EnemySpawnData> m_spawnList;
    std::vector<Enemy*> m_enemies; // 生成したエネミーを覚えておくリスト

	int m_spawnEnemyCount = 0; // スポーンした敵の総数（統計用。制限時間制なので上限は無し）
	int m_maxEnemyCount = 10; // 同時にフィールドへ出せる敵の最大数（ラッシュ中は倍の20になる）
    float m_spawnTimer = 0.0f;
    
	bool m_BossDeathFlag = false; // ボスが死んだかどうかのフラグ
	bool m_isSpawnActive = true; // 敵のスポーンが許可されているかどうかのフラグ
	bool m_isRushMode = false; // ラッシュ中かどうかのフラグ
	
};