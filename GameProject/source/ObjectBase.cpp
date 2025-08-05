#include "ObjectBase.h"
#include "DxLib.h"

ObjectBase::ObjectBase() { //‰Šú‰»
    cgHandle = -1;
    x = 0;
    y = 0;
    w = 0;
    h = 0;
    cgHandleDeleteLock = false;
}

ObjectBase::ObjectBase(const TCHAR* fileName) {
    cgHandle = -1;
    x = 0;
    y = 0;
    w = 0;
    h = 0;
    cgHandleDeleteLock = false;

    Load(fileName);
}

ObjectBase::~ObjectBase() {
    if (cgHandle != -1 && !cgHandleDeleteLock) {
        DeleteGraph(cgHandle);
        cgHandle = -1;
    }
}

void ObjectBase::Init() {}

void ObjectBase::Load(const TCHAR* fileName) {
    cgHandle = LoadGraph(fileName);
}

void ObjectBase::Process(const int key, const int trriger) {}

void ObjectBase::Draw() {
	if(cgHandle != -1) { //•`‰æƒnƒ“ƒhƒ‹‚ª—LŒø‚Èê‡
        DrawGraph(x, y, cgHandle, TRUE);
    }
}
