#pragma once

#include <matjson.hpp>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <chrono>
using namespace cocos2d;
void unallocateNodeId(CCNode* node);

template<typename T>
// as_optional_of<Type>()
std::optional<T> as_optional_of(matjson::Value const& v) {
  if (v.is<T>()) return std::optional(v.as<T>());
  return std::nullopt;
}

template<typename T>
T as_type_or(matjson::Value const& v, T d) {
  return as_optional_of<T>(v).value_or(d);
};
inline int now() {
  return std::chrono::duration_cast<std::chrono::seconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
}

GEODE_NOINLINE uintptr_t getModule(const char* module);
