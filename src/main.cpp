#include <Geode/Geode.hpp> 
#include "WS.hpp"


void start() {
  for (auto* hook : geode::Mod::get()->getHooks()) {
    // hook as late as logically possible
    //
    // the intention is to always let other mods
    // able to prevent devtools from sending events 
    // where in reality it either happens later 
    // or just straight up cancelled
    // (like a dom insertion event that doesnt actually happen) 
    hook->setPriority(geode::Priority::LastPre);
  }
  Protocol::get()->init();
}


$on_mod(Loaded) {
  if (geode::Mod::get()->getSettingValue<bool>("autostart")) {
    geode::queueInMainThread([]{
      start();
    });
  }
}
