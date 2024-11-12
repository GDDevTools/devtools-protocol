#include "state.hpp"
#include "../../../external/mujs/jsi.h"
#undef inline // kill yourself
#include <Geode/DefaultInclude.hpp>
#include <Geode/loader/Log.hpp>

geode::Mod* representer;

static void Console_log(js_State* s) {
  if (js_isstring(s, 1)) {
    std::string j(js_tostring(s,1));
    geode::log::logImpl(geode::Severity::Info, representer, "{}", j);
  }
  js_pushundefined(s);
}

static void Console_debug(js_State* s) {
  if (js_isstring(s, 1)) {
    std::string j(js_tostring(s,1));
    geode::log::logImpl(geode::Severity::Debug, representer, "{}", j);
  }
  js_pushundefined(s);
}

static void Console_warn(js_State* s) {
  if (js_isstring(s, 1)) {
    std::string j(js_tostring(s,1));
    geode::log::logImpl(geode::Severity::Warning, representer, "{}", j);
  }
  js_pushundefined(s);
}

static void Console_error(js_State* s) {
  if (js_isstring(s, 1)) {
    std::string j(js_tostring(s,1));
    geode::log::logImpl(geode::Severity::Error, representer, "{}", j);
  }
  js_pushundefined(s);
}


$execute{
  geode::ModMetadata meta("henrysck075.puppeteer.js");

  meta.setName("JavaScript");
  meta.setVersion(geode::VersionInfo{7,7,7});
  meta.setDescription("The representation of the JavaScript console output from GD DevTools Protocol.");
  meta.setDeveloper(geode::Mod::get()->getDeveloper());

  representer = new geode::Mod(meta);

  initNewClass();
  auto s = getState();
  {
    js_newcfunction(s, Console_log, "console.log", 1);
    js_defproperty(s, -2, "log", JS_DONTENUM);

    js_newcfunction(s, Console_debug, "console.debug", 1);
    js_defproperty(s, -2, "debug", JS_DONTENUM);

    js_newcfunction(s, Console_warn, "console.warn", 1);
    js_defproperty(s, -2, "warn", JS_DONTENUM);

    js_newcfunction(s, Console_error, "console.error", 1);
    js_defproperty(s, -2, "error", JS_DONTENUM);

  }
  //js_newcconstructor(s, new_Console, new_Console, "Console", 0);
  js_defglobal(s, "console", JS_DONTENUM);

  //js_dostring(s, "var console = new Console()");
/*
  js_getglobal(s, "Console");
  if (js_pconstruct(s, 0)) {
    geode::log::error("Error creating Console object.");
  } else {
    auto console = js_toobject(s, -1);
    js_pop(s,1);

    js_pushobject(s, console);
    js_setglobal(s, "console");
  }
*/
}
