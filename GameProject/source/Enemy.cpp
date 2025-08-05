#include "Enemy.h"
#include "DxLib.h"

namespace {
    constexpr auto ENEMY_WIDTH = 32;
    constexpr auto ENEMY_HEIGHT = 32;
    constexpr auto ENEMY_SPEED_X = 8;
    constexpr auto ENEMY_SPEED_Y = 4;
} // namespace

Enemy::Enemy(const int width, const int height, const int cgHandle) : ObjectBase() {
    use = false;
    spd_x = 0;
    spd_y = 0;

    screenWidth = width;
    screenHeight = height;

    this->cgHandle = cgHandle;
}

Enemy::~Enemy() {}

void Enemy::InitPos() {
    x = rand() % screenWidth;
    y = (0 - h);
}

void Enemy::Init() {
    use = true;
    w = ENEMY_WIDTH;
    h = ENEMY_HEIGHT;
    spd_x = ENEMY_SPEED_X;
    spd_y = ENEMY_SPEED_Y;

    InitPos();
}

void Enemy::Process(const int key, const int trriger) {
    // MoveEnemy �ɑ���
    // ���̓G�͎g�p�����H
    if (use == true) {
        // ���̓G�͎g�p���ł���
        // �G�̈ړ�����
        x += spd_x;
        y += spd_y;

        // ���o�E���h�̔���
        if (x < 0 || x + w > screenWidth) {
            spd_x *= -1;
        }

        // ��ʉ��ɏo�锻��
        if (y > screenHeight) {
            InitPos();
        }
    }
}

void Enemy::Draw() {
    if (use == false) {
        return;
    }

    ObjectBase::Draw();
}
