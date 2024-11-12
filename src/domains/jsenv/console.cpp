#include "state.hpp"
#include "../../../external/mujs/jsi.h"
#undef inline // kill yourself
#include <Geode/DefaultInclude.hpp>
#include <Geode/loader/Log.hpp>

static void new_Console(js_State* s) {
}

static void Console_log(js_State* s) {
  if (js_isstring(s, 1)) {
    std::string j(js_tostring(s,1));
    geode::log::info("{}", j);
  }
  js_pushundefined(s);
}

static void Console_debug(js_State* s) {
  if (js_isstring(s, 1)) {
    std::string j(js_tostring(s,1));
    geode::log::debug("{}", j);
  }
  js_pushundefined(s);
}

static void Console_warn(js_State* s) {
  if (js_isstring(s, 1)) {
    std::string j(js_tostring(s,1));
    geode::log::warn("{}", j);
  }
  js_pushundefined(s);
}

static void Console_error(js_State* s) {
  if (js_isstring(s, 1)) {
    std::string j(js_tostring(s,1));
    geode::log::error("{}", j);
  }
  js_pushundefined(s);
}

$execute{
  initNewClass();
  auto s = getState();
  {
    js_newcfunction(s, Console_log, "Console.prototype.log", 1);
    js_defproperty(s, -2, "log", JS_DONTENUM);

    js_newcfunction(s, Console_debug, "Console.prototype.debug", 1);
    js_defproperty(s, -2, "debug", JS_DONTENUM);

    js_newcfunction(s, Console_warn, "Console.prototype.warn", 1);
    js_defproperty(s, -2, "warn", JS_DONTENUM);

    js_newcfunction(s, Console_error, "Console.prototype.error", 1);
    js_defproperty(s, -2, "error", JS_DONTENUM);

  }
  js_newcconstructor(s, new_Console, new_Console, "Console", 0);
  js_defglobal(s, "Console", JS_DONTENUM);

  js_dostring(s, "var console = new Console()");
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
