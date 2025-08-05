#pragma once
#include <vector>
#include "Bullet.h"
#include "ObjectBase.h"
//プレイヤーのことを管理するクラス

class Player : public ObjectBase { // ObjectBaseを継承
public:
    Player(const int width,
           const int height,
           const int cgBullet,
		const TCHAR* fileName);// コンストラクタ
    Player(const int width, const int height, const int cgBullet);
	virtual ~Player();// デストラクタ

	void Init() override;// 初期化処理
	void Process(const int key, const int trriger) override;
    void Draw() override;

	void SetDamage();// ダメージを受ける処理

    int GetSpeed() const { return spd; }
	void SetSpeed(const int spd) { this->spd = spd; }// スピードのゲッターとセッター

	int GetLife() const { return life; }// ライフのゲッター
	void SetLife(const int life) { this->life = life; }// ライフのセッター

	std::vector<Bullet*>& GetBullet() { return bullet; }// 弾のゲッター

    private:
		void AddPlayerBullet();// 弾を追加する処理

    int spd;
    int life;

    int screenWidth;
    int screenHeight;

	std::vector<Bullet*> bullet;// 弾の配列
};
