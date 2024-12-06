#include "state.hpp"
#include <cstddef>
#include <iostream>

TinyJS::Interpreter* mumustate = nullptr;
TinyJS::Interpreter* getState() {
  if (mumustate == nullptr) {
    mumustate = new TinyJS::Interpreter();
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
