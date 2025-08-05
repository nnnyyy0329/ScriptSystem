#pragma once
#include "ObjectBase.h"

class Player;  // �O���錾

class Score : public ObjectBase {
public:
	Score();// �R���X�g���N�^
	// �X�R�A�̏����l��ݒ肷��R���X�g���N�^

	virtual ~Score();// �f�X�g���N�^

	void Draw() override;// �`��֐�

	void AddScore(const int score) { this->score += score; } // �X�R�A�����Z����֐�
	int GetScore() const { return score; }					 // �X�R�A���擾����֐�
	void SetPlayer(Player* player) { this->player = player; }// �v���C���[��ݒ肷��֐�

private:
    int score;
	Player* player;// �v���C���[�ւ̃|�C���^
};
