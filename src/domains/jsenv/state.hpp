#ifndef __puppeteer_js__
#define __puppeteer_js__

#include "../../../external/tinyjs/TinyJS.hpp"

TinyJS::Interpreter* getState();

void nukeState();
#endif


#define $jsMethod(name) static void name(TinyJS::Variable* v, void* userdata)
