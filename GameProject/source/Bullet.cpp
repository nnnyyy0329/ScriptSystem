#include "Bullet.h"

namespace {
    constexpr auto BULLET_WIDTH = 8;
    constexpr auto BULLET_HEIGHT = 16;
    constexpr auto BULLET_SPEED_Y = 16;
} // namespace

Bullet::Bullet(const int cgHandle) : ObjectBase() {
    use = false;
    spd_y = 0;

    this->cgHandle = cgHandle;
}

Bullet::~Bullet() {}

void Bullet::Process(const int key, const int trriger) {
    // MovePlayerBullet に相当
    // この弾は使用中か？
    if (use == true) {
        // 画面上方向へ移動
        y -= spd_y;

        // 画面上から消えたか？
        if (y + h < 0) {
            use = false;
        }
    }
}

void Bullet::Draw() {
    // この弾は使用中か？
    if (use == false) {
        return;
    }

    ObjectBase::Draw();
}

void Bullet::Set(const int x, const int y) {
    // 使っていない。ここを使う
    use = true;
    w = BULLET_WIDTH;
    h = BULLET_HEIGHT;
    this->x = x - w / 2;
    this->y = y - h;
    spd_y = BULLET_SPEED_Y;
}
