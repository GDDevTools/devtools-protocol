#pragma once

#include "../../../../external/mujs/mujs.h"

js_State* getState();

void nukeState();

inline void initNewClass() {
  js_getglobal(getState(), "Object");
	js_getproperty(getState(), -1, "prototype");
}
