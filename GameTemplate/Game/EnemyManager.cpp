#include "stdafx.h"
#include "EnemyManager.h"
#include "Enemy.h"
void EnemyManager::Init() {
	Enemy*enemy1=NewGO<Enemy>(1);
	enemy1->m_position = { 7000.0f, 0.0f, -12000.0f };
	enemy1->m_firstPos = enemy1->m_position;
	Enemy*enemy2=NewGO<Enemy>(2);
	enemy2->m_position = { 7100.0f, 0.0f, -3500.0f };
	enemy2->m_firstPos = enemy2->m_position;
	Enemy*enemy3=NewGO<Enemy>(3);
	enemy3->m_position = { 7000.0f, 0.0f, 100.0f };
	enemy3->m_firstPos = enemy3->m_position;
	Enemy*enemy4=NewGO<Enemy>(4);
	enemy4->m_position = { 7000.0f, 0.0f, -300.0f };
	enemy4->m_firstPos = enemy4->m_position;
	Enemy* enemy5 = NewGO<Enemy>(5);
	enemy5->m_position = { 4200.0f, 0.0f, -7100.0f };
	enemy5->m_firstPos = enemy5->m_position;
	Enemy* enemy6 = NewGO<Enemy>(6);
	enemy6->m_position = { 4400.0f, 0.0f, -7100.0f };
	enemy6->m_firstPos = enemy6->m_position;
	Enemy* enemy7 = NewGO<Enemy>(7);
	enemy7->m_position = { -7000.0f, 0.0f, -7000.0f };
	enemy7->m_firstPos = enemy7->m_position;
	Enemy* enemy8 = NewGO<Enemy>(8);
	enemy8->m_position = { -4000.0f, 0.0f, -7000.0f };
	enemy8->m_firstPos = enemy8->m_position;
	Enemy* enemy9 = NewGO<Enemy>(9);
	enemy9->m_position = { -10000.0f, 0.0f, -7000.0f };
	enemy9->m_firstPos = enemy9->m_position;
	Enemy* enemy10 = NewGO<Enemy>(10);
	enemy10->m_position = { 7000.0f, 0.0f, -3500.0f };
	enemy10->m_firstPos = enemy10->m_position;
	Enemy* enemy11 = NewGO<Enemy>(11);
	enemy11->m_position = { 7200.0f, 0.0f, -3500.0f };
	enemy11->m_firstPos = enemy11->m_position;
	Enemy* enemy12 = NewGO<Enemy>(12);
	enemy12->m_position = { 7300.0f, 0.0f, -3500.0f };
	enemy12->m_firstPos = enemy12->m_position;
	Enemy* enemy13 = NewGO<Enemy>(13);
	enemy13->m_position = { 7400.0f, 0.0f, -3500.0f };
	enemy13->m_firstPos = enemy13->m_position;
}
