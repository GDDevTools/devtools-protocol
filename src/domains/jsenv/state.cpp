#include "state.hpp"
#include <cstddef>
#include <iostream>

CTinyJS* mumustate = nullptr;
CTinyJS* getState() {
  if (mumustate == nullptr) {
    mumustate = new CTinyJS();
  }
  return mumustate;
}

void nukeState() {
  delete mumustate;
};  

void nothing(CFunctionsScopePtr const &v, void *userdata) {
  v->setReturnVar(newScriptVarUndefined(mumustate));
}
