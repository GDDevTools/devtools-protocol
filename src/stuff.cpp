#include "stuff.hpp"

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
