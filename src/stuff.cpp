#include "stuff.hpp"

int highestNodeId = 0;
/// a bunch of ub(?)
std::map<CCNode*, int> nodeIds;
std::queue<int> unused;
int nodeIdOf(CCNode* node) {
  if (unused.empty()) {
    return highestNodeId++;
  } else {
    int ret = unused.front();
    unused.pop();
    return ret;
  }
}

void unallocateNodeId(CCNode* node) {
  unused.push(nodeIds[node]);
  nodeIds.erase(node);
}


uintptr_t getModule(const char* module) {
  #ifdef GEODE_IS_WINDOWS
  return reinterpret_cast<uintptr_t>(GetModuleHandleA(module));
  #elif defined(GEODE_IS_ANDROID)
  auto handle = dlopen(module, RTLD_LAZY | RTLD_NOLOAD);

  // JNI_OnLoad is present on all versions of GD
  auto sym = dlsym(handle, "JNI_OnLoad");
  assert(sym != nullptr);

  Dl_info p;
  auto dlAddrRes = dladdr(sym, &p);
  assert(dlAddrRes != 0);

  return reinterpret_cast<std::uintptr_t>(p.dli_fbase);
  #endif
}