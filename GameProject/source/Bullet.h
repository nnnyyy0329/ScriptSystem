#pragma once
#include "ObjectBase.h"

class Bullet : public ObjectBase {
public:
    Bullet(const int cgHandle);
    virtual ~Bullet();

    void Process(const int key, const int trriger) override;
    void Draw() override;

    void Set(const int x, const int y);

    bool IsUse() const { return use; }
    void SetUse(const bool use) { this->use = use; }

    int GetSpeedY() const { return spd_y; }
    void SetSpeedY(const int spd_y) { this->spd_y = spd_y; }

private:
    bool use;
    int spd_y;
};
