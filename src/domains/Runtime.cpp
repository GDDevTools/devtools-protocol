// ?

#include "Geode/DefaultInclude.hpp"
#include "jsenv/state.hpp"
#include "../WS.hpp"
#include <matjson.hpp>
#include "../../external/mujs/jsi.h"

struct RemoteObject {
  std::string type;
  std::string clsName;
  matjson::Value value;
  std::string desc;
};

struct matjson::Serialize<RemoteObject> {
  static matjson::Value to_json(const RemoteObject& r) {
    return matjson::Object {
      {"type", r.type},
      {"className", r.clsName},
      {"value", r.value},
      {"description", r.desc}
    };
  }
};

$domainMethod(evaluate) {
  auto s = getState();
  js_loadstring(s, "[string]", params["expression"].as_string().c_str());
	js_pushglobal(s);
	js_call(s, 0);
  auto ret = js_tovalue(s, 1);
  std::string type;
  switch (ret->t.type) {
    case JS_TBOOLEAN:
      type = "boolean";
    case JS_TOBJECT:
      if (ret->u.object->type == JS_CARRAY) type = "array";
      else type = "object";
  }
  return geode::Ok(matjson::Object{});
}

$execute {
  auto p = Protocol::get();
  p->registerFunction("Runtime.evaluate", &evaluate, {"expression"});
}
