#include "state.hpp"
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

class bob {
  /// @brief This is only ever be destructed on game close
  ~bob() {
    // shut up geode
    std::cout << "Closing MuJS state.";
    nukeState();
  }
};

static bob not_bob;