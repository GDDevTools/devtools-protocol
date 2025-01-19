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

template<typename otter, typename T>
std::vector<T> map_(const std::vector<otter>& input, std::function<T(const otter&)> p) {
  std::vector<T> the;
  the.reserve(input.size());
  for (auto& i : input) {
    the.push_back(p(i));
  }
  return the;
}
template<matjson::CanDeserialize T>
std::vector<T> deserializeJsonArray(const std::vector<matjson::Value>& inp) {
  return map_<matjson::Value, T>(inp, [](const matjson::Value& e){return matjson::Serialize<T>::fromJson(e).unwrap();});
}

GEODE_NOINLINE uintptr_t getModule(const char* module);

template<typename T>
inline T* getParentOfType(cocos2d::CCNode* n) {
  cocos2d::CCNode* p = n->getParent();
  while (p!=nullptr) {
    if (auto tcp = geode::cast::typeinfo_cast<T*>(p)) return tcp;
    p = p->getParent();
  }
  return nullptr;
}