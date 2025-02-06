#include "../WS.hpp"
#include <Geode/Result.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/Loader.hpp>

using namespace geode;

$domainMethod(disableMod) {
  if (
    auto m = Loader::get()->getLoadedMod(params["mod"].asString().unwrap())
  )
    m->disable();
  return emptyResponse();
}
$domainMethod(enableMod) {
  if (
    auto m = Loader::get()->getInstalledMod(params["mod"].asString().unwrap())
  )
    m->enable();
  return emptyResponse();
}
$domainMethod(getSettingsItems) {
  matjson::Value ret;
  if (
    auto m = Loader::get()->getInstalledMod(params["mod"].asString().unwrap())
  ) {
    for (auto& p : m->getMetadata().getSettings()) {
      ret.set(p.first,p.second);
    }
  }
  return geode::Ok(matjson::makeObject({
    {"data", ret}
  }));
}
$domainMethod(setSettings) {
  if (
    auto m = Loader::get()->getInstalledMod(params["mod"].asString().unwrap())
  ) {
    for (auto& [k,v] : params["settings"]) {
#define set(type) m->setSettingValue<type>(k, v.as<type>().unwrap())
      if (v.isBool()) set(bool);
      else if (v.asInt().isOk()) set(int64_t);
      else if (v.asDouble().isOk() || v.isNumber()) set(double);
      else if (v.isString()) set(std::string);
#undef set
    }
    return emptyResponse();
  }
  return errors::invalidParameter("Mod not installed.");
}

$on_mod(Loaded) {
  auto p = Protocol::get();
  p->registerFunction("Mod.disableMod", &disableMod, {"mod"});
  p->registerFunction("Mod.enableMod", &disableMod, {"mod"});
  p->registerFunction("Mod.getSettingsItems", &getSettingsItems, {"mod"});
  p->registerFunction("Mod.setSettings", &setSettings, {"mod", "settings"});
}
