#include "stdafx.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "Boss.h"

namespace {
    const float SPORN_RANGE = 1000.0f;//ランダム生成範囲を設定
    const float SPAWN_LIMIT_TIME = 1.0f; // 1秒という数値を定数化
    const int MAX_ENEMIES = 15;          // 最大数も定数化
}

void EnemyManager::Init() {

    m_enemies.clear();
    m_spawnTimer = 0.0f;     
    // 最初に数体出しておきたい場合はここでSpawnを呼ぶ
}

void EnemyManager::SpawnEnemies() {

  //エネミーを20体スポーンさせるまで、1秒ごとにスポーンさせる
  //if (m_spawnEnemyCount >= m_maxEnemyCount) {
		//return; // 既に最大数スポーンしているので、これ以上スポーンさせない
  //}
    
  // 1. タイマーを進める
  m_spawnTimer += g_gameTime->GetFrameDeltaTime();

  // 2. タイマーが一定時間を超えたらスポーン処理を行う
  if (m_spawnTimer >= SPAWN_LIMIT_TIME) {
      m_spawnTimer = 0.0f; // タイマーをリセット

      // 3. 敵の数が最大数未満なら新しい敵をスポーン
      if (m_enemies.size() < MAX_ENEMIES) {

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
	//ボスを呼びだされるまではザコ敵をスポーンさせる
    if (m_isSpawnActive) {
        SpawnEnemies();
    }

}