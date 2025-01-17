#include <Geode/Geode.hpp> 
#include <climits>
#include "WS.hpp"

$domainMethod(getProtInfo) {
  std::string c = geode::utils::file::readFile(geode::Mod::get()->getResourcesPath() / "protocols.json");
  return geode::Ok(
    r.unwrap()
  );
}
void start() {
  for (auto* hook : geode::Mod::get()->getHooks()) {
    // hook as late as possible
    // so any mod willing not to call / delay original
    // can prevent events from sending
    hook->setPriority(INT_MAX);
  }
  Protocol::get()->init();
}


$execute {
  if (geode::Mod::get()->getSettingValue<bool>("autostart")) start();
}