// ?

// TODO: this is not fucking valid what the fuck
#include "Geode/DefaultInclude.hpp"
#include "jsenv/state.h"
#include "../WS.hpp"

$domainMethod(evaluate) {
  js_dostring(getState(), params["expression"].as_string().c_str());
  return geode::Ok(matjson::Object{});
}

$execute {
  auto p = Protocol::get();
  p->registerFunction("Runtime.evaluate", &evaluate, {"expression"});
}
