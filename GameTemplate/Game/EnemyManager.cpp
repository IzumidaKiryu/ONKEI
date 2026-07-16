#include "stdafx.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "Boss.h"

namespace {
    const float SPORN_RANGE = 1000.0f;//ランダム生成範囲を設定
    const float SPAWN_LIMIT_TIME = 1.0f; // 通常時のスポーン間隔
    const float RUSH_SPAWN_LIMIT_TIME = 0.5f; // ラッシュ時のスポーン間隔（湧く速さを倍にする）
}

//自分が生成した敵を道連れにする。
//DeleteGO(m_enemyManager)だけでは敵が野良で生き残り、リザルトにHPゲージが残ってしまうため。
EnemyManager::~EnemyManager() {
    ClearAllEnemies();
}

//撃破数と撃破スコアを加算する。
//スコア＝基礎点 ×ラッシュ倍率 ×チェイン倍率。倒し続けるほど美味しくして手を止めさせない。
void EnemyManager::CountUpDeathCount()
{
    m_deathCount++;

    //連続撃破を伸ばす。次を倒すまでの猶予をここでリセットする。
    m_chainCount++;
    m_chainTimer = CHAIN_WINDOW_TIME;

    float score = (float)KILL_SCORE_PER_ENEMY;
    if (m_isRushMode) {
        score *= 2.0f;
    }
    score *= GetChainMultiplier();

    m_killScore += (int)score;
}

//現在のチェイン数によるスコア倍率。1体目は等倍で、以降1体ごとに増える。
float EnemyManager::GetChainMultiplier() const
{
    if (m_chainCount <= 1) {
        return 1.0f;
    }
    const float mul = 1.0f + (m_chainCount - 1) * CHAIN_MULTIPLIER_STEP;
    return (mul > CHAIN_MULTIPLIER_MAX) ? CHAIN_MULTIPLIER_MAX : mul;
}

void EnemyManager::Init() {

    m_enemies.clear();
    m_spawnTimer = 0.0f;
	//前回のプレイの記録が残らないようにリセットする
	m_deathCount = 0;
	m_killScore = 0;
	m_spawnEnemyCount = 0;
	m_chainCount = 0;
	m_chainTimer = 0.0f;
	m_isRushMode = false;
	m_isSpawnActive = true;
    // 最初に数体出しておきたい場合はここでSpawnを呼ぶ
}

//敵を1体だけ生成する。スポーン位置はランダム。
void EnemyManager::SpawnOneEnemy() {

    // 乱数で -0.5 ～ 0.5 の範囲を作り、範囲(SPORN_RANGE)をかける
    float rx = ((float)rand() / RAND_MAX) - 0.5f;
    float rz = ((float)rand() / RAND_MAX) - 0.5f;

    Vector3 randomPos = {
        rx * SPORN_RANGE,
        0.0f,
        rz * SPORN_RANGE
    };

    // 名前をユニークにする（デバッグしやすくなります）
    static int totalCount = 0;
    char name[32];
    sprintf_s(name, "enemy_%d", totalCount++);

    Enemy* enemy = NewGO<Enemy>(0, name);
    enemy->m_position = randomPos;
    enemy->m_firstPos = randomPos;

    // リストに追加
    m_enemies.push_back(enemy);

    m_spawnEnemyCount++;
}

void EnemyManager::SpawnEnemies() {

  //制限時間制になったので、総スポーン数の上限は無し。
  //ラッシュ中（残り30秒以降）はフィールド上の最大数と湧く速さを倍にして難易度を上げる。
  //敵1体はモデル＋物理2つ分のコストがあるので、最大数はここで頭打ちにしておかないと重くなる。
  const int maxEnemies = m_isRushMode ? m_maxEnemyCount * 2 : m_maxEnemyCount;
  const float spawnInterval = m_isRushMode ? RUSH_SPAWN_LIMIT_TIME : SPAWN_LIMIT_TIME;

  // 1. フィールドが埋まっているなら何もしない
  if ((int)m_enemies.size() >= maxEnemies) {
      return;
  }

  // 2. タイマーを進める
  m_spawnTimer += g_gameTime->GetFrameDeltaTime();

  // 3. タイマーが一定時間を超えたらスポーン処理を行う
  if (m_spawnTimer >= spawnInterval) {
      m_spawnTimer = 0.0f; // タイマーをリセット
      SpawnOneEnemy();
  }
}

// 敵から「消えるよ」と通知が来たらリストから消す
void EnemyManager::OnEnemyDestroy(Enemy* enemy) {
    auto it = std::find(m_enemies.begin(), m_enemies.end(), enemy);
    if (it != m_enemies.end()) {
        m_enemies.erase(it);
    }
}

void EnemyManager::BossSpawn()
{
    // 1. スポーン機能を止める（フラグを折る）
    m_isSpawnActive = false;

    // 2. ここで呼び出す！これでザコ敵が全員消える
    ClearAllEnemies();

    m_boss = NewGO<Boss>(0, "boss");

    // 3. ボスの初期位置を設定（ステージの奥など）
    Vector3 bossPos = { 0.0f, 0.0f, 0.0f };
    m_boss->SetPosition(bossPos);
}

void EnemyManager::ClearAllEnemies() {
    // リストのコピーを作成してループを回す（削除中のリスト変更を防ぐため）
    auto currentEnemies = m_enemies;
    for (auto enemy : currentEnemies) {
        DeleteGO(enemy);
    }
    m_enemies.clear();
	m_spawnTimer = 0.0f; // タイマーもリセット
	m_BossDeathFlag = false; // ボスの死亡フラグもリセット
}

void EnemyManager::Update() {

	//チェインの猶予を減らし、時間切れなら連続撃破を打ち切る。
	//リズムゲーム中はEnemyManagerごとDeactivateされるのでここは進まず、チェインは維持される。
	if (m_chainCount > 0) {
		m_chainTimer -= g_gameTime->GetFrameDeltaTime();
		if (m_chainTimer <= 0.0f) {
			m_chainTimer = 0.0f;
			m_chainCount = 0;
		}
	}

	//ボスを呼びだされるまではザコ敵をスポーンさせる
    if (m_isSpawnActive) {
        SpawnEnemies();
    }

}