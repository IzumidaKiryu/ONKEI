#include "stdafx.h"
#include "EnemyManager.h"
void EnemyManager::Init() {
	Enemy*enemy1=NewGO<Enemy>(1);
	enemy1->m_position = { 300.0f, 0.0f, 300.0f };
	Enemy*enemy2=NewGO<Enemy>(2);
	enemy2->m_position = { 200.0f, 0.0f, 200.0f };
	Enemy*enemy3=NewGO<Enemy>(3);
	enemy3->m_position = { 100.0f, 0.0f, 100.0f };
	Enemy*enemy4=NewGO<Enemy>(4);
	enemy4->m_position = { -300.0f, 0.0f, -300.0f };
}
