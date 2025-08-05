/*
*
* Commnet
*
*/
#include "GameMain.h"
#include "DxLib.h"


namespace {
    // ��ʐݒ�
    constexpr auto SCREEN_W = 1280;    ///< ��ʂ̉��𑜓x
    constexpr auto SCREEN_H = 720;     ///< ��ʂ̏c�𑜓x
    constexpr auto SCREEN_DEPTH = 32;  ///< �P�h�b�g������̃r�b�g��
} // namespace

// �v���O������ WinMain ����n�܂�܂�
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow) {
    // ������
    // -------------------------------------------------------------------------
    // �c�w���C�u��������������
    SetGraphMode(SCREEN_W, SCREEN_H, SCREEN_DEPTH);  // ��ʂ̉𑜓x����1280�~�c720�A1�h�b�g������32�r�b�g�Ɏw�肷��

    ChangeWindowMode(true);    // �E�B���h�E���[�h�Ɏw�肷��

    if (DxLib_Init() == -1) {  // �G���[���N�����璼���ɏI��
        return -1;
    }

    SetDrawScreen(DX_SCREEN_BACK);  // �`����ʂ𗠉�ʂɃZ�b�g

    // �Q�[�����C��
    GameMain* gameMain = new GameMain();

    gameMain->Init();

    // ���C�����[�v
    while (ProcessMessage() == 0) {
        gameMain->Input();

        if (!gameMain->Process()) {
            break;
        }

        ClearDrawScreen();
        gameMain->Draw();
        ScreenFlip();
    }

    delete gameMain;

    // ���
    // ---------------------------------------------------------------------------
    DxLib_End();  // �c�w���C�u�����g�p�̏I������

    // �I��
    return 0;
}

