#include "Score.h"
#include "DxLib.h"
#include "Player.h"

namespace {
    constexpr auto SCORE_POS_X = 32;
    constexpr auto SCORE_POS_Y = 32;
    auto scoreColor = GetColor(255, 0, 0);
} // namespace

Score::Score() : ObjectBase() {
    score = 0;
    player = nullptr;
}

Score::~Score() {}

void Score::Draw() {
    if (player == nullptr) {
        return;
    }

    // ライフ、スコア表示
    DrawFormatString(SCORE_POS_X, SCORE_POS_Y, scoreColor, "Life:%d   Score:%d", player->GetLife(), score);
}
