#pragma once
#include "ObjectBase.h"

// 背景画像をロードして描画だけするクラス
class BackGround : public ObjectBase {
public:
    BackGround(const TCHAR* fileName);
    virtual ~BackGround();
};
