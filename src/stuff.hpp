#pragma once

#include <matjson.hpp>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <chrono>
using namespace cocos2d;
void unallocateNodeId(CCNode* node);

inline int now() {
  return std::chrono::duration_cast<std::chrono::seconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
}

GEODE_NOINLINE uintptr_t getModule(const char* module);
