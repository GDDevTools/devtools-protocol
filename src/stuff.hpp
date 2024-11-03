#pragma once

#include <matjson.hpp>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <chrono>
#include <queue>
using namespace cocos2d;

int nodeIdOf(CCNode* node);

void unallocateNodeId(CCNode* node);

template<typename T>
// as_optional_of<Type>()
std::optional<T> as_optional_of(matjson::Value const& v) {
  if (v.is<T>()) return std::optional(v.as<T>());
  return std::nullopt;
}

template<typename T>
T as_type_or(matjson::Value const& v, T d) {
  if (v.is<T>()) return v.as<T>();
  return d;
};
inline int now() {
  return std::chrono::duration_cast<std::chrono::seconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
}
