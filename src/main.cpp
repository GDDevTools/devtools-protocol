#include <Geode/Geode.hpp> 
#include <climits>
#include "WS.hpp"

$execute {
  for (auto* hook : geode::Mod::get()->getHooks()) {
    // hook as late as possible
    // so any mod willing not to call / delay original
    // can prevent events from sending
    hook->setPriority(INT_MAX);
  }
  Protocol::get()->init();
}
