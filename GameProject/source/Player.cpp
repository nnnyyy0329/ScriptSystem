#include "Player.h"
#include "DxLib.h"

namespace {
    constexpr auto PLAYER_WIDTH = 48;
    constexpr auto PLAYER_HEIGHT = 48;
    constexpr auto PLAYER_SPEED = 8;
    constexpr auto PLAYER_LIFE = 5;
    constexpr auto BULLET_MAX = 3;
} // namespace

Player::Player(const int width,
               const int height,
               const int cgBullet,
               const TCHAR* fileName) : ObjectBase(fileName) {
    spd = 0;
    life = 0;

    screenWidth = width;
    screenHeight = height;

    for (auto i = 0; i < BULLET_MAX; ++i)
    {
        bullet.emplace_back(new Bullet(cgBullet));
    }
}

Player::Player(const int width, const int height, const int cgBullet) : ObjectBase() {
    spd = 0;
    life = 0;

    screenWidth = width;
    screenHeight = height;

    for (auto i = 0; i < BULLET_MAX; ++i) {
        bullet.emplace_back(new Bullet(cgBullet));
    }
}

Player::~Player() {
    for (auto&& bt : bullet) {
        delete bt;
    }

    bullet.clear();
}

void Player::Init() {
    w = PLAYER_WIDTH;
    h = PLAYER_HEIGHT;
    x = (screenWidth - w) / 2;
    y = (screenHeight - h) / 2;
    spd = PLAYER_SPEED;
    life = PLAYER_LIFE;
}

void Player::Process(const int key, const int trriger) {
    // �L�[���͂𔻒肵�āA��l�����ړ�������
    if (key & PAD_INPUT_LEFT) {
        x -= spd;
    }
    if (key & PAD_INPUT_RIGHT) {
        x += spd;
    }
    if (key & PAD_INPUT_UP) {
        y -= spd;
    }
    if (key & PAD_INPUT_DOWN) {
        y += spd;
    }

    // ��ʂ̊O�ɏo�Ȃ��悤�ɂ���
    if (x < 0) {
        x = 0;
    }
    if (x + w > screenWidth) {
        x = screenWidth - w;
    }
    if (y < 0) {
        y = 0;
    }
    if (y + h > screenHeight) {
        y = screenHeight - h;
    }

    // �e�̔���
    if (trriger & PAD_INPUT_A) {
        AddPlayerBullet();
    }

    for (auto& bt : bullet) {
        bt->Process(key, trriger);
    }
}

void Player::Draw() {
    ObjectBase::Draw();

    for (auto& bt : bullet) {
        bt->Draw();
    }
}

void Player::SetDamage() {
    y = screenHeight - h; // ���[�Ɉړ�
    life--;               // ���C�t�����炷
}

void Player::AddPlayerBullet() {
    // �g���Ă��Ȃ��z���T��
    for (auto& bt : bullet) {
        if (bt->IsUse() == false) {
            // �g���Ă��Ȃ��B�������g��
            // �J�n�ʒu���v���C���[���W����Z�o����
            bt->Set(x + w / 2, y);
            break;
        }
    }
}
