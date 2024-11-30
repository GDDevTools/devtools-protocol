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
  static matjson::Value toJson(const RemoteObject& r) {
    return matjson::makeObject( {
      {"type", r.type},
      {"className", r.clsName},
      {"value", r.value},
      {"description", r.desc}
    });
  }
};

matjson::Value jsvalToJsonVal(js_Value* val) {
  matjson::Value ret;
  if (val->t.type == JS_TUNDEFINED) {
    return nullptr;
  } else if (val->t.type == JS_TNULL) {
    ret = nullptr;
  } else if (val->t.type == JS_TBOOLEAN) {
    ret = (bool)val->u.boolean;
  } else if (val->t.type == JS_TNUMBER) {
    ret = val->u.number;
  } else if (val->t.type == JS_TLITSTR) {
    ret = val->u.litstr;
  } else if (val->t.type == JS_TMEMSTR) {
    ret = std::string(val->u.memstr->p);
  } else if (val->t.type == JS_TSHRSTR) {
    ret = std::string(val->u.shrstr);
  } else if (val->t.type == JS_TOBJECT) {
    matjson::Value wrap;
    std::string type;
    if (val->u.object->type == JS_CARRAY) {
      auto arr = matjson::Value::array();
      type = "array";
      auto a = val->u.object->u.a;
      for (int i = 0; i < a.length; i++) {
        arr.push(jsvalToJsonVal(&a.array[i]));
      }
      wrap = arr;
    }
    else if (val->u.object->type == JS_CFUNCTION) {
      auto f = val->u.object->u.f;
      auto func = f.function;
      wrap = matjson::makeObject({
        {"script", func->script}
      });
    }
    else if (val->u.object->type == JS_COBJECT) {
      matjson::Value obj;
      auto props = val->u.object->properties;
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
