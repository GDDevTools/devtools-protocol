#pragma once

#include <matjson.hpp>
#include <Geode/cocos/base_nodes/CCNode.h>
using namespace cocos2d;

inline int nodeIdOf(CCNode* node) {
  return reinterpret_cast<intptr_t>(node);
}

template<typename T>
T as_type_or(matjson::Value const& v, T d) {
  if (v.is<T>()) return v.as<T>();
  return d;
};