#include <Geode/Geode.hpp> 
#include <climits>
#include "WS.hpp"

$domainMethod(getProtInfo) {
  std::string c = geode::utils::file::readString(geode::Mod::get()->getResourcesDir() / "protocols.json").unwrap();
  return geode::Ok(c);
}
void start() {
  for (auto* hook : geode::Mod::get()->getHooks()) {
    // hook as late as possible
    // so any mod willing not to call / delay original
    // can prevent events from sending
    // when they were initially called
    hook->setPriority(INT_MAX);
  }
  Protocol::get()->init();
}


$execute {
  if (geode::Mod::get()->getSettingValue<bool>("autostart")) {
    geode::queueInMainThread([]{
      start();
    });
  }
}
