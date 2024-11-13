#pragma once

#include "../../../external/mujs/mujs.h"
#include "../../../../external/mujs/jsi.h"

js_State* getState();

void nukeState();

inline void initNewClass() {
  js_getglobal(getState(), "Object");
	js_getproperty(getState(), -1, "prototype");
}

#define $jsMethod(name) static void name(js_State* s)