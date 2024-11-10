#include <Geode/Geode.hpp> 
#include <climits>
#include "WS.hpp"

$domainMethod(getProtInfo) {
  std::string c = cocos2d::CCFileUtils::get()->fullPathForFilename("protocols.json"_spr,true);
  auto r = geode::utils::file::readJson(std::filesystem::path(c));
  if (r.isErr()) {
    return errors::internalError("?");
  }
  return geode::Ok(
    r.unwrap().as_object()
  );
}
$execute {
  for (auto* hook : geode::Mod::get()->getHooks()) {
    // hook as late as possible
    // so any mod willing not to call / delay original
    // can prevent events from sending
    hook->setPriority(INT_MAX);
  }
  Protocol::get()->init();
}
