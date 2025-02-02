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
template<ScriptObjectType T>
void validateContext(const CScriptVarScopePtr &v) {
  if (dynamic_cast<T*>(v.getVar()->findChild("this")) == nullptr) {
    mumustate->throwException(TypeError, "Illegal invocation");
  }
}