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
    // MovePlayerBullet �ɑ���
    // ���̒e�͎g�p�����H
    if (use == true) {
        // ��ʏ�����ֈړ�
        y -= spd_y;

        // ��ʏォ����������H
        if (y + h < 0) {
            use = false;
        }
    }
}

void Bullet::Draw() {
    // ���̒e�͎g�p�����H
    if (use == false) {
        return;
    }

    ObjectBase::Draw();
}

void Bullet::Set(const int x, const int y) {
    // �g���Ă��Ȃ��B�������g��
    use = true;
    w = BULLET_WIDTH;
    h = BULLET_HEIGHT;
    this->x = x - w / 2;
    this->y = y - h;
    spd_y = BULLET_SPEED_Y;
}
