#include "../WS.hpp"
#include "Geode/utils/Result.hpp"
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/Loader.hpp>

using namespace geode;

$domainMethod(disableMod) {
  if (
    auto m = Loader::get()->getLoadedMod(params["mod"].as_string())
  )
    m->disable();
  return geode::Ok(matjson::Value::object());
}
$domainMethod(enableMod) {
  if (
    auto m = Loader::get()->getInstalledMod(params["mod"].as_string())
  )
    m->enable();
  return geode::Ok(matjson::Value::object());
}
$domainMethod(getSettingsItems) {
  matjson::Object ret;
  if (
    auto m = Loader::get()->getInstalledMod(params["mod"].as_string())
  ) {
    for (auto& p : m->getMetadata().getSettingsV3()) {
      ret.insert(p);
    }
  }
  return geode::Ok(matjson::makeObject({
    {"data", ret}
  });
}
$domainMethod(setSettings) {
  if (
    auto m = Loader::get()->getInstalledMod(params["mod"].as_string())
  ) {
    for (auto& p : params["settings"].as_object()) {
#define set(type) m->setSettingValue<type>(p.first, p.second.as<type>())
      if (p.second.is_bool()) set(bool);
      if (p.second.is<int>()) set(int64_t);
      if (p.second.is<double>() || p.second.is<float>()) set(double);
      if (p.second.is_string()) set(std::string);
#undef set
    }
    return geode::Ok(matjson::Value::object());
  }
  return errors::invalidParameter("Mod not installed.");
}

$execute {
  auto p = Protocol::get();
  p->registerFunction("Mods.disableMod", &disableMod, {"mod"});
  p->registerFunction("Mods.enableMod", &disableMod, {"mod"});
  p->registerFunction("Mods.getSettingsItems", &getSettingsItems, {"mod"});
  p->registerFunction("Mods.setSettings", &setSettings, {"mod", "settings"});
}
