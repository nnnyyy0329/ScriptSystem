#pragma once
#include <vector>
#include "ObjectBase.h"

class Player; // �O���錾
class Enemy;  // �O���錾
class Score;  // �O���錾

class GameMain {
public:
    GameMain();
    virtual ~GameMain();

    void Init();
    void Input();
    bool Process();
    void Draw();

private:
    void Save() const;
    void SavePlayer(const Player* player) const;
    void SaveEnemyList(const std::vector<Enemy*> enemyList) const;
    void SaveScore(const Score* score) const;

    void Load();
    void DeleteOldObjects();
    Player* LoadPlayer() const;
    std::vector<Enemy*> LoadEnemyList() const;
    Score* LoadScore() const;

    void Create();
    void SetupScore();
    bool IsHitBox(const int x1,
                  const int y1,
                  const int w1,
                  const int h1,
                  const int x2,
                  const int y2,
                  const int w2,
                  const int h2) const;
    void HitCheckPlayerEnemy(Player* player) const;
    void HitCheckEnemyPlayerBullet(Player* player) const;
    Player* GetPlayer() const;
    Score* GetScore() const;

    int inputKey;
    int inputTrg;

    // C++ �� STL �ł��� std::vector ��z��̕ς��Ɏg��
	std::vector<ObjectBase*> objectList;// �I�u�W�F�N�g�̃��X�g

    // Save / Load �ׂ̈ɑ��₵�������o
    int cgBullet;
    int cgEnemy;

    // �|�[�Y(&Save/Load)�@�\
    bool isPause;
    bool isSelectSave;
};
