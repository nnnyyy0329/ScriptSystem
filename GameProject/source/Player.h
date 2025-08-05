#pragma once
#include <vector>
#include "Bullet.h"
#include "ObjectBase.h"
//�v���C���[�̂��Ƃ��Ǘ�����N���X

class Player : public ObjectBase { // ObjectBase���p��
public:
    Player(const int width,
           const int height,
           const int cgBullet,
		const TCHAR* fileName);// �R���X�g���N�^
    Player(const int width, const int height, const int cgBullet);
	virtual ~Player();// �f�X�g���N�^

	void Init() override;// ����������
	void Process(const int key, const int trriger) override;
    void Draw() override;

	void SetDamage();// �_���[�W���󂯂鏈��

    int GetSpeed() const { return spd; }
	void SetSpeed(const int spd) { this->spd = spd; }// �X�s�[�h�̃Q�b�^�[�ƃZ�b�^�[

	int GetLife() const { return life; }// ���C�t�̃Q�b�^�[
	void SetLife(const int life) { this->life = life; }// ���C�t�̃Z�b�^�[

	std::vector<Bullet*>& GetBullet() { return bullet; }// �e�̃Q�b�^�[

    private:
		void AddPlayerBullet();// �e��ǉ����鏈��

    int spd;
    int life;

    int screenWidth;
    int screenHeight;

	std::vector<Bullet*> bullet;// �e�̔z��
};
