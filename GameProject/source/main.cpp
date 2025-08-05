/*
*
* Commnet
*
*/
#include "GameMain.h"
#include "DxLib.h"


namespace {
    // 画面設定
    constexpr auto SCREEN_W = 1280;    ///< 画面の横解像度
    constexpr auto SCREEN_H = 720;     ///< 画面の縦解像度
    constexpr auto SCREEN_DEPTH = 32;  ///< １ドットあたりのビット数
} // namespace

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow) {
    // 初期化
    // -------------------------------------------------------------------------
    // ＤＸライブラリ初期化処理
    SetGraphMode(SCREEN_W, SCREEN_H, SCREEN_DEPTH);  // 画面の解像度を横1280×縦720、1ドットあたり32ビットに指定する

    ChangeWindowMode(true);    // ウィンドウモードに指定する

    if (DxLib_Init() == -1) {  // エラーが起きたら直ちに終了
        return -1;
    }

    SetDrawScreen(DX_SCREEN_BACK);  // 描画先画面を裏画面にセット

    // ゲームメイン
    GameMain* gameMain = new GameMain();

    gameMain->Init();

    // メインループ
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

    // 解放
    // ---------------------------------------------------------------------------
    DxLib_End();  // ＤＸライブラリ使用の終了処理

    // 終了
    return 0;
}

