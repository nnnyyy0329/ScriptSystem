#include "BackGround.h"

// 継承先クラスのコンストラクタに続けて ": 基底クラスコンストラクタ"
// と呼び出せる
BackGround::BackGround(const TCHAR* fileName) : ObjectBase(fileName) {
	//コンストラクタの処理
}

BackGround::~BackGround() {}
