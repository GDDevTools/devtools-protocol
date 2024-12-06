// ?

#include "Geode/DefaultInclude.hpp"
#include "jsenv/state.hpp"
#include "../WS.hpp"
#include <matjson.hpp>
#include "../../external/tinyjs/TinyJS.hpp"
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
  static matjson::Value toJson(const RemoteObject& r) {
    return matjson::makeObject( {
      {"type", r.type},
      {"className", r.clsName},
      {"value", r.value},
      {"description", r.desc}
    });
  }
};

matjson::Value jsvalToJsonVal(CScriptVar* val) {
  matjson::Value ret;
  if (val->isUndefined()) {
    return nullptr;
  } else if (val->isNull()) {
    ret = nullptr;
  } else if (val->isBool()) {
    ret = val->getBool();
  } else if (val->isNumber()) {
    ret = val->getDouble();
  } else if (val->isString()) {
    ret = val->getString();
  }
  else if (val->isArray()) {
    auto arr = matjson::Value::array();
    for (int i = 0; i < val->getArrayLength(); i++) {
      arr.push(jsvalToJsonVal(val->getArrayIndex(i).getVar()));
    }
    ret = arr;
  } else {
    matjson::Value wrap;
    std::string type;
    if (val->isFunction()) {
      auto f = val->u.object->u.f;
      auto func = f.function;
      wrap = matjson::makeObject({
        {"script", func->script}
      });
    } else {
      matjson::Value obj;
      auto props = val->Childs;
      for (int i = 0; i < val->u.object->count; i++) {
        auto p = props[i];
        obj[p.name] = jsvalToJsonVal(&p.value);
      }
    }
    ret = matjson::makeObject({
      {"type", type},
      {"value", wrap}
    });
  }
}
$domainMethod(evaluate) {
  auto s = getState();
  js_loadeval(s, "[string]", params["expression"].asString().unwrapOr("").c_str());
	js_pushglobal(s);
	js_call(s, 0);
  js_pop(s,1);
  return geode::Ok(matjson::Value::object());
}

$execute {
  auto p = Protocol::get();
  p->registerFunction("Runtime.evaluate", &evaluate, {"expression"});
}
