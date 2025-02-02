#pragma once

#include "../external/tinyjs/TinyJS.hpp"

CTinyJS* getState();

void nukeState();

#define $jsMethod(name) static void name(CFunctionsScopePtr const& v, void* userdata)

void nothing(CFunctionsScopePtr const &v, void *userdata);

template<class T>
concept ScriptObjectType = std::is_base_of_v<CScriptVarObject, T>;

template<ScriptObjectType T>
inline void validateContext(const CScriptVarScopePtr &v) {
  if (dynamic_cast<T*>(v.getVar()->findChild("this")->getVarPtr().getVar()) == nullptr) {
    getState()->throwException(TypeError, "Illegal invocation");
  }
}