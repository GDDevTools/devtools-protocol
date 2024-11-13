// ?

#include "Geode/DefaultInclude.hpp"
#include "jsenv/state.hpp"
#include "../WS.hpp"
#include <matjson.hpp>
#include "../../external/mujs/jsi.h"
#include "../../external/mujs/mujs.h"
#undef inline

struct RemoteObject {
  std::string type;
  std::string clsName;
  matjson::Value value;
  std::string desc;

  //static RemoteObject describe(js_Value* val);
};

template<>
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

//RemoteObject RemoteObject::describe(js_Value* val) {};


$domainMethod(evaluate) {
  auto s = getState();
  js_loadstring(s, "[string]", params["expression"].as_string().c_str());
	js_pushglobal(s);
	js_call(s, 0);
  js_pop(s,1);
  return geode::Ok(matjson::Object{});
}

$execute {
  auto p = Protocol::get();
  p->registerFunction("Runtime.evaluate", &evaluate, {"expression"});
}
