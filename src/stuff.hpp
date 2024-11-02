#pragma once

#include <matjson.hpp>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <chrono>
using namespace cocos2d;

inline int nodeIdOf(CCNode* node) {
  return reinterpret_cast<intptr_t>(node);
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
