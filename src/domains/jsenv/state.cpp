#include "state.h"
#include <cstddef>

js_State* state = nullptr;
js_State* getState() {
  if (state == nullptr) {
    state = js_newstate(NULL, NULL, JS_STRICT);
  }
  return state;
}

void nukeState() {
  delete state;
};  

