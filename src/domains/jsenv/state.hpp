#ifndef __puppeteer_js__
#define __puppeteer_js__

#include "../../../external/mujs/jsi.h"
#include "../../../external/mujs/mujs.h"

js_State* getState();

void nukeState();
#endif

inline void initNewClass() {
  js_getglobal(getState(), "Object");
	js_getproperty(getState(), -1, "prototype");
}

#define $jsMethod(name) static void name(js_State* s)
