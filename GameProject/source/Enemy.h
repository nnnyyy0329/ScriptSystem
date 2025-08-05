#pragma once
#include "ObjectBase.h"
#include "Bullet.h"
#include <vector>

class Enemy : public ObjectBase {
public:
    Enemy(const int width, const int height, const int cgHandle);
    virtual ~Enemy();

    void Init() override;
    void Process(const int key, const int trriger) override;
    void Draw() override;

    bool IsUse() const { return use; }
    void SetUse(const bool use) { this->use = use; }

    int GetSpeedX() const { return spd_x; }
    int GetSpeedY() const { return spd_y; }

    void SetSpeedX(const int spd_x) { this->spd_x = spd_x; }
    void SetSpeedY(const int spd_y) { this->spd_y = spd_y; }

private:
    void InitPos();

    bool use;
    int spd_x;
    int spd_y;

    int screenWidth;
    int screenHeight;

	std::vector<Bullet*> bullet; // “G‚Ì”z—ñ
};
