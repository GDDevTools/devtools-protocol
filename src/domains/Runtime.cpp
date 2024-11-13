// ?

#include "Geode/DefaultInclude.hpp"
#include "jsenv/state.hpp"
#include "../WS.hpp"
#include <matjson.hpp>
#include "../../external/mujs/jsi.h"
#include "../../external/mujs/mujs.h"

struct RemoteObject {
  std::string type;
  std::string clsName;
  matjson::Value value;
  std::string desc;
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
      matjson::Array arr;
      type = "array";
      auto a = val->u.object->u.a;
      for (int i = 0; i < a.length; i++) {
        arr.push_back(jsvalToJsonVal(&a.array[i]));
      }
      wrap = arr;
    }
    else if (val->u.object->type == JS_CFUNCTION) {
      auto f = val->u.object->u.f;
      auto func = f.function;
      wrap = matjson::Object{
        {"script", func->script}
      };
    }
    else if (val->u.object->type == JS_COBJECT) {
      matjson::Object obj;
      auto props = val->u.object->properties;
      for (int i = 0; i < val->u.object->count; i++) {
        auto p = props[i];
        obj[p.name] = jsvalToJsonVal(&p.value);
      }
    }
    ret = matjson::Object{
      {"type", type},
      {"value", wrap}
    };
  }

  return ret;
}
$domainMethod(evaluate) {
  auto s = getState();
  js_loadstring(s, "[string]", params["expression"].as_string().c_str());
	js_pushglobal(s);
	js_call(s, 0);
  return geode::Ok(matjson::Object{});
}

$execute {
  auto p = Protocol::get();
  p->registerFunction("Runtime.evaluate", &evaluate, {"expression"});
}
