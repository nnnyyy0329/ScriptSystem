#pragma once
#include "ObjectBase.h"

class Player;  // 前方宣言

class Score : public ObjectBase {
public:
	Score();// コンストラクタ
	// スコアの初期値を設定するコンストラクタ

	virtual ~Score();// デストラクタ

	void Draw() override;// 描画関数

	void AddScore(const int score) { this->score += score; } // スコアを加算する関数
	int GetScore() const { return score; }					 // スコアを取得する関数
	void SetPlayer(Player* player) { this->player = player; }// プレイヤーを設定する関数

private:
    int score;
	Player* player;// プレイヤーへのポインタ
};
