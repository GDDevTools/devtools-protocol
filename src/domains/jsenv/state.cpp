#include "state.hpp"
#include <cstddef>
#include <iostream>

js_State* mumustate = nullptr;
js_State* getState() {
  if (mumustate == nullptr) {
    mumustate = js_newstate(NULL, NULL, JS_STRICT);
  }
  return mumustate;
}

void nukeState() {
  delete mumustate;
};  

class bob {
public:
  /// @brief This is only ever be destructed on game close
  ~bob() {
    // shut up geode
    std::cout << "Closing MuJS state.";
    nukeState();
  }
};

static bob not_bob;
