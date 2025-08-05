#include "GameMain.h"
#include <fstream>
#include "BackGround.h"
#include "DxLib.h"
#include "Enemy.h"
#include "Player.h"
#include "Score.h"
#include "nlohmann/json.hpp"

namespace {// 名前空間を使うと、関数や変数の名前が衝突しないようにできる
    constexpr auto DISP_W = 1280;  // 画面の横解像度
    constexpr auto DISP_H = 720;   // 画面の縦解像度
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

    // 基底クラスの情報を保存する
    // 基底クラスという事は、全クラスで共通なので処理を関数化
    void SaveObjectBase(nlohmann::json& json, const ObjectBase* objectBase) {
        json["x"] = objectBase->GetX();
        json["y"] = objectBase->GetY();
        json["w"] = objectBase->GetW();
        json["h"] = objectBase->GetH();
    }

    // 基底クラスの情報を読み込む
    // 基底クラスという事は、全クラスで共通なので処理を関数化
    void LoadObjectBase(nlohmann::json& json, ObjectBase* objectBase) {
        int x, y, w, h;

        // JSON から値を取得
		json.at(_T("x")).get_to(x);
        json.at(_T("y")).get_to(y);
        json.at(_T("w")).get_to(w);
        json.at(_T("h")).get_to(h);

		// 取得した値を基底クラスのメンバ変数にセット
        objectBase->SetX(x);
        objectBase->SetY(y);
        objectBase->SetW(w);
        objectBase->SetH(h);
    }
} // namespace

GameMain::GameMain() { // コンストラクタ
    inputKey = 0;
    inputTrg = 0;
    cgBullet = -1;
    cgEnemy = -1;
    isPause = false;
    isSelectSave = true;
}

GameMain::~GameMain() { // デストラクタ
    for (auto& object : objectList) { //
        object->SetCgHandleDeleteLock(false);
        delete object;
    }

    objectList.clear();
}

void GameMain::Create() {
    // emplace_back() std::vector に要素を追加するメソッド
    // ObjectBase* (基底クラスのポインタ)型に対して、派生先のクラスで new
    // が出来るのがポリモーフィズムの特徴
    objectList.emplace_back(new BackGround(_T("res/bg_space.png")));

    // 同じ画像を何度もロードしない様にする
    cgBullet = LoadGraph(_T("res/Bullet_player.png"));

    // std::vector は動的に要素を追加できる
    objectList.emplace_back(new Player(DISP_W, DISP_H, cgBullet, _T("res/player00.png")));

    cgEnemy = LoadGraph(_T("res/enemy_a00.png"));

    for (auto i = 0; i < ENEMY_NUM; ++i) {
        objectList.emplace_back(new Enemy(DISP_W, DISP_H, cgEnemy));
    }

    objectList.emplace_back(new Score());
}

void GameMain::SetupScore() {
    // Playerclass とScore class のポインタを扱う
    Player* player = GetPlayer();
    Score* score = GetScore();

    if (player != nullptr && score != nullptr) {
        score->SetPlayer(player);
    }
}

void GameMain::Init() {
    Create();

    // 範囲 for 文
    // 受ける変数は auto で、右辺の型に合わせて変数の型を自動にすると便利
    // & は C++ の参照渡し。参照渡しを使うと、配列の要素を直接操作できる
    for (auto& object : objectList) {
        // この動作がポリモーフィズムの特徴
        object->Init(); // new したクラスの Init メソッドになる
    }

    SetupScore();
}

void GameMain::Input() {
    // キーの入力、トリガ入力を得る
    int keyOld = inputKey;

    inputKey = GetJoypadInputState(DX_INPUT_KEY_PAD1); // キー入力を取得
    // キーのトリガ情報生成（押した瞬間しか反応しないキー情報）
    inputTrg = (inputKey ^ keyOld) & inputKey;
}

bool GameMain::Process() {
    // ESC Key 押しでゲーム終了
    if (inputKey & PAD_INPUT_9) {
        return false;
    }

    // Q Key 押しでポーズ
    if (inputTrg & PAD_INPUT_7) {
        isPause = !isPause;
    }

    if (isPause) {
        if ((inputTrg & PAD_INPUT_UP) || (inputTrg & PAD_INPUT_DOWN)) {
            isSelectSave = !isSelectSave;
        }

        // Space Key 押しで決定
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

    // この動作がポリモーフィズムの特徴
    for (auto& object : objectList) {
        object->Process(inputKey, inputTrg); // new したクラスの Process メソッドになる
    }

    // プレイヤーと敵の当たり処理
    HitCheckPlayerEnemy(player);

    // 敵とプレイヤーの弾の当たり処理
    HitCheckEnemyPlayerBullet(player);

    return true;
}

void GameMain::Draw() {
    // この動作がポリモーフィズムの特徴
    for (auto& object : objectList) {
        object->Draw(); // new したクラスの Draw メソッドになる
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

// 当たり判定用。2つのboxが当たったかを判定
// 当たっていたらtrue, 当たっていなかったらfalseを返す
bool GameMain::IsHitBox(const int x1,
                        const int y1,
                        const int w1,
                        const int h1, // ひとつめのbox 左上(x,y), 大きさw,h
                        const int x2,
                        const int y2,
                        const int w2,
                        const int h2  // ふたつめのbox 左上(x,y), 大きさw,h
) const {
    if (x1 < x2 + w2 && x2 < x1 + w1    // x方向の判定
        && y1 < y2 + h2 && y2 < y1 + h1 // y方向の判定
    ) {
        // 2つのboxは当たっている
        return true;
    }

    // 2つのboxは当たっていない
    return false;
}

Player* GameMain::GetPlayer() const {
    for (auto& object : objectList) {
        // C++ の機能である dynamic_cast
        // で、基底クラスのポインタを派生クラスのポインタに変換する
        auto player = dynamic_cast<Player*>(object);

        if (player != nullptr) {
            return player;
        }
    }

    return nullptr;
}

Score* GameMain::GetScore() const {
    for (auto& object : objectList) {
        // C++ の機能である dynamic_cast
        // で、基底クラスのポインタを派生クラスのポインタに変換する
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

        // この敵は使用中か？
        if (enemy->IsUse() == false) {
            continue;
        }

        // プレイヤーと敵の当たり判定
        if (IsHitBox(player->GetX(), player->GetY(), player->GetW(), player->GetH(), // プレイヤーを囲むBox
                     enemy->GetX(), enemy->GetY(), enemy->GetW(), enemy->GetH()      // 敵[i]を囲むBox
                     ) != false) {
            // 当たった
            enemy->SetUse(false);  // この敵を消す
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

        // この敵は使用中か？
        if (enemy->IsUse() == false) {
            continue;
        }

        for (auto& bt : player->GetBullet()) {
            // この弾は使用中か？
            if (bt->IsUse() == false) {
                continue;
            }

            if (IsHitBox(enemy->GetX(), enemy->GetY(), enemy->GetW(), enemy->GetH(), // 敵[i]を囲むBox
                         bt->GetX(), bt->GetY(), bt->GetW(), bt->GetH()              // 弾[j]を囲むBox
                         ) != false) {
                // 当たった
                enemy->SetUse(false);  // この敵を消す
                bt->SetUse(false);     // 弾を消す
                score->AddScore(10);   // スコアアップ
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
        // objectList から現在の Player を削除
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
        // objectList から現在の Score を削除
        auto remove = std::remove(objectList.begin(), objectList.end(), score);
        objectList.erase(remove, objectList.end());

        score->SetCgHandleDeleteLock(true);
        delete score;
    }
}

void GameMain::Load() {
    auto player = LoadPlayer();

    if (player == nullptr) {
        return; // Player のセーブデータがない(以降の処理もしない)
    }

    auto enemyList = LoadEnemyList();

    auto score = LoadScore();

    if (enemyList.size() > 0 && score != nullptr) {
        DeleteOldObjects();

        // SetupScore と同等の処理
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
