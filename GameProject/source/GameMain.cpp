#include "GameMain.h"
#include <fstream>
#include "BackGround.h"
#include "DxLib.h"
#include "Enemy.h"
#include "Player.h"
#include "Score.h"
#include "nlohmann/json.hpp"

namespace {// ���O��Ԃ��g���ƁA�֐���ϐ��̖��O���Փ˂��Ȃ��悤�ɂł���
    constexpr auto DISP_W = 1280;  // ��ʂ̉��𑜓x
    constexpr auto DISP_H = 720;   // ��ʂ̏c�𑜓x
    constexpr auto ENEMY_NUM = 30;

    constexpr auto PauseX = 550;
    constexpr auto PauseY = 250;
	constexpr auto SaveX = 550; 
	constexpr auto SaveY = 350; 
    constexpr auto LoadX = 550;
    constexpr auto LoadY = 450;

    constexpr auto PauseText = _T("PAUSE");
    constexpr auto SaveText = _T("Save");
    constexpr auto LoadText = _T("Load");

    const auto PauseColor = GetColor(0, 0, 255);
    const auto MenuColor = GetColor(128, 128, 128);
    const auto SelectColor = GetColor(196, 196, 64);

    // ���N���X�̏���ۑ�����
    // ���N���X�Ƃ������́A�S�N���X�ŋ��ʂȂ̂ŏ������֐���
    void SaveObjectBase(nlohmann::json& json, const ObjectBase* objectBase) {
        json["x"] = objectBase->GetX();
        json["y"] = objectBase->GetY();
        json["w"] = objectBase->GetW();
        json["h"] = objectBase->GetH();
    }

    // ���N���X�̏���ǂݍ���
    // ���N���X�Ƃ������́A�S�N���X�ŋ��ʂȂ̂ŏ������֐���
    void LoadObjectBase(nlohmann::json& json, ObjectBase* objectBase) {
        int x, y, w, h;

        // JSON ����l���擾
		json.at(_T("x")).get_to(x);
        json.at(_T("y")).get_to(y);
        json.at(_T("w")).get_to(w);
        json.at(_T("h")).get_to(h);

		// �擾�����l�����N���X�̃����o�ϐ��ɃZ�b�g
        objectBase->SetX(x);
        objectBase->SetY(y);
        objectBase->SetW(w);
        objectBase->SetH(h);
    }
} // namespace

GameMain::GameMain() { // �R���X�g���N�^
    inputKey = 0;
    inputTrg = 0;
    cgBullet = -1;
    cgEnemy = -1;
    isPause = false;
    isSelectSave = true;
}

GameMain::~GameMain() { // �f�X�g���N�^
    for (auto& object : objectList) { //
        object->SetCgHandleDeleteLock(false);
        delete object;
    }

    objectList.clear();
}

void GameMain::Create() {
    // emplace_back() std::vector �ɗv�f��ǉ����郁�\�b�h
    // ObjectBase* (���N���X�̃|�C���^)�^�ɑ΂��āA�h����̃N���X�� new
    // ���o����̂��|�����[�t�B�Y���̓���
    objectList.emplace_back(new BackGround(_T("res/bg_space.png")));

    // �����摜�����x�����[�h���Ȃ��l�ɂ���
    cgBullet = LoadGraph(_T("res/Bullet_player.png"));

    // std::vector �͓��I�ɗv�f��ǉ��ł���
    objectList.emplace_back(new Player(DISP_W, DISP_H, cgBullet, _T("res/player00.png")));

    cgEnemy = LoadGraph(_T("res/enemy_a00.png"));

    for (auto i = 0; i < ENEMY_NUM; ++i) {
        objectList.emplace_back(new Enemy(DISP_W, DISP_H, cgEnemy));
    }

    objectList.emplace_back(new Score());
}

void GameMain::SetupScore() {
    // Playerclass ��Score class �̃|�C���^������
    Player* player = GetPlayer();
    Score* score = GetScore();

    if (player != nullptr && score != nullptr) {
        score->SetPlayer(player);
    }
}

void GameMain::Init() {
    Create();

    // �͈� for ��
    // �󂯂�ϐ��� auto �ŁA�E�ӂ̌^�ɍ��킹�ĕϐ��̌^�������ɂ���ƕ֗�
    // & �� C++ �̎Q�Ɠn���B�Q�Ɠn�����g���ƁA�z��̗v�f�𒼐ڑ���ł���
    for (auto& object : objectList) {
        // ���̓��삪�|�����[�t�B�Y���̓���
        object->Init(); // new �����N���X�� Init ���\�b�h�ɂȂ�
    }

    SetupScore();
}

void GameMain::Input() {
    // �L�[�̓��́A�g���K���͂𓾂�
    int keyOld = inputKey;

    inputKey = GetJoypadInputState(DX_INPUT_KEY_PAD1); // �L�[���͂��擾
    // �L�[�̃g���K��񐶐��i�������u�Ԃ����������Ȃ��L�[���j
    inputTrg = (inputKey ^ keyOld) & inputKey;
}

bool GameMain::Process() {
    // ESC Key �����ŃQ�[���I��
    if (inputKey & PAD_INPUT_9) {
        return false;
    }

    // Q Key �����Ń|�[�Y
    if (inputTrg & PAD_INPUT_7) {
        isPause = !isPause;
    }

    if (isPause) {
        if ((inputTrg & PAD_INPUT_UP) || (inputTrg & PAD_INPUT_DOWN)) {
            isSelectSave = !isSelectSave;
        }

        // Space Key �����Ō���
        if (inputTrg & PAD_INPUT_10) {
            if (isSelectSave) {
                Save();
            } else {
                Load();
            }
        }

        return true;
    }

    Player* player = GetPlayer();

    if (player == nullptr || player->GetLife() <= 0) {
        return true;
    }

    // ���̓��삪�|�����[�t�B�Y���̓���
    for (auto& object : objectList) {
        object->Process(inputKey, inputTrg); // new �����N���X�� Process ���\�b�h�ɂȂ�
    }

    // �v���C���[�ƓG�̓����菈��
    HitCheckPlayerEnemy(player);

    // �G�ƃv���C���[�̒e�̓����菈��
    HitCheckEnemyPlayerBullet(player);

    return true;
}

void GameMain::Draw() {
    // ���̓��삪�|�����[�t�B�Y���̓���
    for (auto& object : objectList) {
        object->Draw(); // new �����N���X�� Draw ���\�b�h�ɂȂ�
    }

    if (isPause) {
        auto orignalSize = GetFontSize();

        SetFontSize(64);
        DrawString(PauseX, PauseY, PauseText, PauseColor);
        DrawString(SaveX, SaveY, SaveText, isSelectSave ? SelectColor : MenuColor);
        DrawString(LoadX, LoadY, LoadText, isSelectSave ? MenuColor : SelectColor);
        SetFontSize(orignalSize);
    }
}

// �����蔻��p�B2��box�������������𔻒�
// �������Ă�����true, �������Ă��Ȃ�������false��Ԃ�
bool GameMain::IsHitBox(const int x1,
                        const int y1,
                        const int w1,
                        const int h1, // �ЂƂ߂�box ����(x,y), �傫��w,h
                        const int x2,
                        const int y2,
                        const int w2,
                        const int h2  // �ӂ��߂�box ����(x,y), �傫��w,h
) const {
    if (x1 < x2 + w2 && x2 < x1 + w1    // x�����̔���
        && y1 < y2 + h2 && y2 < y1 + h1 // y�����̔���
    ) {
        // 2��box�͓������Ă���
        return true;
    }

    // 2��box�͓������Ă��Ȃ�
    return false;
}

Player* GameMain::GetPlayer() const {
    for (auto& object : objectList) {
        // C++ �̋@�\�ł��� dynamic_cast
        // �ŁA���N���X�̃|�C���^��h���N���X�̃|�C���^�ɕϊ�����
        auto player = dynamic_cast<Player*>(object);

        if (player != nullptr) {
            return player;
        }
    }

    return nullptr;
}

Score* GameMain::GetScore() const {
    for (auto& object : objectList) {
        // C++ �̋@�\�ł��� dynamic_cast
        // �ŁA���N���X�̃|�C���^��h���N���X�̃|�C���^�ɕϊ�����
        auto score = dynamic_cast<Score*>(object);

        if (score != nullptr) {
            return score;
        }
    }

    return nullptr;
}

void GameMain::HitCheckPlayerEnemy(Player* player) const {
    if (player == nullptr) {
        return;
    }

    for (auto& object : objectList) {
        auto enemy = dynamic_cast<Enemy*>(object);

        if (enemy == nullptr) {
            continue;
        }

        // ���̓G�͎g�p�����H
        if (enemy->IsUse() == false) {
            continue;
        }

        // �v���C���[�ƓG�̓����蔻��
        if (IsHitBox(player->GetX(), player->GetY(), player->GetW(), player->GetH(), // �v���C���[���͂�Box
                     enemy->GetX(), enemy->GetY(), enemy->GetW(), enemy->GetH()      // �G[i]���͂�Box
                     ) != false) {
            // ��������
            enemy->SetUse(false);  // ���̓G������
            player->SetDamage();
        }
    }
}

void GameMain::HitCheckEnemyPlayerBullet(Player* player) const {
    Score* score = GetScore();

    if (player == nullptr || score == nullptr) {
        return;
    }

    for (auto& object : objectList) {
        auto enemy = dynamic_cast<Enemy*>(object);

        if (enemy == nullptr) {
            continue;
        }

        // ���̓G�͎g�p�����H
        if (enemy->IsUse() == false) {
            continue;
        }

        for (auto& bt : player->GetBullet()) {
            // ���̒e�͎g�p�����H
            if (bt->IsUse() == false) {
                continue;
            }

            if (IsHitBox(enemy->GetX(), enemy->GetY(), enemy->GetW(), enemy->GetH(), // �G[i]���͂�Box
                         bt->GetX(), bt->GetY(), bt->GetW(), bt->GetH()              // �e[j]���͂�Box
                         ) != false) {
                // ��������
                enemy->SetUse(false);  // ���̓G������
                bt->SetUse(false);     // �e������
                score->AddScore(10);   // �X�R�A�A�b�v
            }
        }
    }
}

void GameMain::Save() const {
    std::vector<Enemy*> enemyList;

    for (auto& object : objectList) {
        auto player = dynamic_cast<Player*>(object);

        if (player != nullptr) {
            SavePlayer(player);
        }

        auto enemy = dynamic_cast<Enemy*>(object);

        if (enemy != nullptr) {
            enemyList.emplace_back(enemy);
        }

        auto score = dynamic_cast<Score*>(object);

        if (score != nullptr) {
            SaveScore(score);
        }
    }

    if (!enemyList.empty()) {
        SaveEnemyList(enemyList);
    }
}

void GameMain::SavePlayer(const Player* player) const {
    nlohmann::json playerJson;

    SaveObjectBase(playerJson, player);

    playerJson[_T("spd")] = player->GetSpeed();
    playerJson[_T("life")] = player->GetLife();
    playerJson[_T("screenWidth")] = DISP_W;
    playerJson[_T("screenHeight")] = DISP_H;

    nlohmann::json playerBulletListJson;

    auto getBulletPlayer = const_cast<Player*>(player);

    for (const auto& bullet : getBulletPlayer->GetBullet()) {
        nlohmann::json bulletJson;

        SaveObjectBase(bulletJson, bullet);

        bulletJson[_T("use")] = bullet->IsUse();
        bulletJson[_T("spd_y")] = bullet->GetSpeedY();

        playerBulletListJson.push_back(bulletJson);
    }

    playerJson[_T("bullet")] = playerBulletListJson;

    std::ofstream ofs(_T("save/player.json"), std::ios::out | std::ios::trunc);

    ofs << playerJson.dump(4);
    ofs.close();
}

void GameMain::SaveScore(const Score* score) const {
    nlohmann::json scoreJson;

    SaveObjectBase(scoreJson, score);

    scoreJson[_T("score")] = score->GetScore();

    std::ofstream ofs(_T("save/score.json"), std::ios::out | std::ios::trunc);

    ofs << scoreJson.dump(4);
    ofs.close();
}

void GameMain::SaveEnemyList(const std::vector<Enemy*> enemyList) const {
    nlohmann::json enemyListJson;

    for (const auto& enemy : enemyList) {
        nlohmann::json enemyJson;

        SaveObjectBase(enemyJson, enemy);

        enemyJson[_T("use")] = enemy->IsUse();
        enemyJson[_T("spd_x")] = enemy->GetSpeedX();
        enemyJson[_T("spd_y")] = enemy->GetSpeedY();
        enemyJson[_T("screenWidth")] = DISP_W;
        enemyJson[_T("screenHeight")] = DISP_H;

        enemyListJson.push_back(enemyJson);
    }

    std::ofstream ofs(_T("save/enemyList.json"),
                      std::ios::out | std::ios::trunc);

    ofs << enemyListJson.dump(4);
    ofs.close();
}

void GameMain::DeleteOldObjects() {
    auto player = GetPlayer();

    if (player != nullptr) {
        // objectList ���猻�݂� Player ���폜
        auto remove = std::remove(objectList.begin(), objectList.end(), player);
        objectList.erase(remove, objectList.end());

        for (auto& bullet : player->GetBullet()) {
            bullet->SetCgHandleDeleteLock(true);
        }

        player->SetCgHandleDeleteLock(true);
        delete player;
    }

    auto removeList = std::remove_if(
        objectList.begin(), objectList.end(), [](ObjectBase* object) {
            auto enemy = dynamic_cast<Enemy*>(object);

            if (enemy != nullptr) {
                enemy->SetCgHandleDeleteLock(true);
                delete enemy;

                return true;
            }

            return false;
        });

    objectList.erase(removeList, objectList.end());

    auto score = GetScore();

    if (score != nullptr) {
        // objectList ���猻�݂� Score ���폜
        auto remove = std::remove(objectList.begin(), objectList.end(), score);
        objectList.erase(remove, objectList.end());

        score->SetCgHandleDeleteLock(true);
        delete score;
    }
}

void GameMain::Load() {
    auto player = LoadPlayer();

    if (player == nullptr) {
        return; // Player �̃Z�[�u�f�[�^���Ȃ�(�ȍ~�̏��������Ȃ�)
    }

    auto enemyList = LoadEnemyList();

    auto score = LoadScore();

    if (enemyList.size() > 0 && score != nullptr) {
        DeleteOldObjects();

        // SetupScore �Ɠ����̏���
        score->SetPlayer(player);

        objectList.emplace_back(player);

        for (auto& enemy : enemyList) {
            objectList.emplace_back(enemy);
        }

        objectList.emplace_back(score);
    }
}

Player* GameMain::LoadPlayer() const {
    std::ifstream ifsPlayer(_T("save/player.json"));

    if (!ifsPlayer) {
        return nullptr;
    }

    nlohmann::json playerJson;

    ifsPlayer >> playerJson;

    Player* newPlayer = new Player(DISP_W, DISP_H, cgBullet);
    auto playerHandle = GetPlayer()->GetCgHandle();

    newPlayer->SetCgHandle(playerHandle);
    LoadObjectBase(playerJson, newPlayer);

    int spd, life;

    playerJson.at(_T("spd")).get_to(spd);
    playerJson.at(_T("life")).get_to(life);

    newPlayer->SetSpeed(spd);
    newPlayer->SetLife(life);

    auto count = 0;
    auto bulletListJson = playerJson.at(_T("bullet"));

    for (auto& bulletJson : bulletListJson) {
        auto bullet = newPlayer->GetBullet()[count];

        LoadObjectBase(bulletJson, bullet);

        bool use;
        int spd_y;

        bulletJson.at(_T("use")).get_to(use);
        bulletJson.at(_T("spd_y")).get_to(spd_y);

        bullet->SetUse(use);
        bullet->SetSpeedY(spd_y);

        count++;
    }

    return newPlayer;
}

std::vector<Enemy*> GameMain::LoadEnemyList() const {
    std::vector<Enemy*> enemyList;
    std::ifstream ifsEnemyList(_T("save/enemyList.json"));

    if (!ifsEnemyList) {
        return enemyList;
    }

    nlohmann::json enemyListJson;

    ifsEnemyList >> enemyListJson;

    for (auto& enemyJson : enemyListJson) {
        auto newEnemy = new Enemy(DISP_W, DISP_H, cgEnemy);

        LoadObjectBase(enemyJson, newEnemy);

        bool use;
        int spd_x, spd_y;

        enemyJson.at(_T("use")).get_to(use);
        enemyJson.at(_T("spd_x")).get_to(spd_x);
        enemyJson.at(_T("spd_y")).get_to(spd_y);

        newEnemy->SetUse(use);
        newEnemy->SetSpeedX(spd_x);
        newEnemy->SetSpeedY(spd_y);

        enemyList.emplace_back(newEnemy);
    }

    return enemyList;
}

Score* GameMain::LoadScore() const {
    std::ifstream ifsScore(_T("save/score.json"));

    if (!ifsScore) {
        return nullptr;
    }

    nlohmann::json scoreJson;

    ifsScore >> scoreJson;

    Score* newScore = new Score();

    LoadObjectBase(scoreJson, newScore);

    int score;

    scoreJson.at(_T("score")).get_to(score);

    newScore->AddScore(score);

    return newScore;
}
