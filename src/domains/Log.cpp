#include "../WS.hpp"
#include <Geode/utils/addresser.hpp>
#include <Geode/loader/Hook.hpp>
#include <Geode/modify/Traits.hpp>
#include <Geode/loader/Log.hpp>
#include <Geode/loader/Mod.hpp>
#include "../stuff.hpp"

void vlogImplHook(geode::Severity sev, geode::Mod* mod, fmt::string_view format, fmt::format_args args) {
  geode::log::vlogImpl(sev, mod, format, args);
  std::string severityStr;
  switch (sev) {
    case geode::Severity::Debug:
      severityStr = "verbose";
      break;
    case geode::Severity::Info:
      severityStr = "info";
      break;
    case geode::Severity::Warning:
      severityStr = "warning";
      break;
    case geode::Severity::Error:
      severityStr = "error";
      break;
    default:
      severityStr = "?????";
      break;
  }
  fireEvent("Log.entryAdded", matjson::makeObject({
    {"entry", matjson::makeObject({
      {"source", mod->getID()},
      {"level", severityStr},
      {"text", fmt::vformat(format, args)},
      {"timestamp", now()}
    })}
  }));
}

geode::Hook* vlogHook;

$domainMethod(disableLog) {
  if (vlogHook) {
    if (vlogHook->disable().isOk())
      return emptyResponse();
    return errors::internalError("Cannot disable vlogImpl hook.");
  }
  return errors::internalError("vlogImpl hook not available.");
}
$domainMethod(enableLog) {
  if (vlogHook) {
    if (vlogHook->enable().isOk())
      return emptyResponse();
    // Err isn't returned in these functions but we'll see
    return errors::internalError("Cannot enable vlogImpl hook.");
  }
  return errors::internalError("vlogImpl hook not available.");
}

$on_mod(Loaded) {
  vlogHook = geode::Mod::get()->hook( 
    reinterpret_cast<void*>(geode::addresser::getNonVirtual(
      geode::modifier::Resolve<
        geode::Severity, geode::Mod*, fmt::string_view, fmt::format_args
      >::func(&geode::log::vlogImpl)
    )),
    &vlogImplHook,
    "geode::log::vlogImpl",
    tulip::hook::TulipConvention::Thiscall
  ).unwrapOr(nullptr);
  if (vlogHook) vlogHook->disable();
  auto p = Protocol::get();
  p->registerFunction("Log.disable", &disableLog);
  p->registerFunction("Log.enable", &enableLog);
}
