// ?

#include "Geode/DefaultInclude.hpp"
#include "../WS.hpp"
#include <matjson.hpp>
#include <optional>
#include "jsenv/state.hpp"
#undef inline

struct Preview {
  std::string type;
  std::string subtype;
  std::string className;
  std::string desc;

  Preview(CScriptVar* val) {
    if (false) {}
    else if (val->isUndefined()) type = "undefined";
    else if (val->isString())    type = "string";
    else if (val->isNumber())    type = "number";
    else if (val->isBool())      type = "boolean";
    else if (val->isFunction())  type = "function";
    else {
      type = "object";
      className = "idk";
      if (false) {}
      else if (val->isArray())     subtype = "array";
      else if (val->isNull())      subtype = "null";
      else if (val->isGenerator()) subtype = "generator";
      else if (val->isIterator())  subtype = "iterator";
    }

  };
};

struct ObjectPreview : public Preview {
  matjson::Value props;
  ObjectPreview(CScriptVar* val) : Preview(val) {}
};

template<>
struct matjson::Serialize<Preview> {
  static matjson::Value toJson(const Preview& p) {
    auto ret = matjson::makeObject( {
      {"type", p.type},
      {"description", p.desc}
    });

    return ret;
  }
};

template<>
struct matjson::Serialize<ObjectPreview> {
  static matjson::Value toJson(const ObjectPreview& o) {
    auto ret = matjson::Serialize<Preview>::toJson(o);
    ret["properties"] = o.props;

    return ret;
  }
};

struct PropertyPreview : public Preview {
  std::optional<ObjectPreview> valuePreview;
  std::string value;
  PropertyPreview(CScriptVar* val) : Preview(val) {
    value = val->toString();
    if (type == "object") 
      valuePreview = std::make_optional(ObjectPreview(val));
    else
      valuePreview = std::nullopt;
  }
};

template<>
struct matjson::Serialize<PropertyPreview> {
  static matjson::Value toJson(const PropertyPreview& p) {
    auto ret = matjson::Serialize<Preview>::toJson(p);
    if (p.valuePreview.has_value())
      ret["valuePreview"] = matjson::Serialize<ObjectPreview>::toJson(p.valuePreview.value());

    return ret;
  }
};

struct RemoteObject : public Preview {
  matjson::Value value;

  RemoteObject(CScriptVar* val) : Preview(val) {
    if (false) {}
    else if (type=="object") {
      value = matjson::Serialize<ObjectPreview>::toJson(ObjectPreview(val));
    }
    else if (type=="string") value = val->toString();
  }
};
template<>
struct matjson::Serialize<RemoteObject> {
  static matjson::Value toJson(const RemoteObject& r) {
    auto ret = matjson::Serialize<Preview>::toJson(r);
    if (ret["type"] == "object") {
      ret["preview"] = r.value;
    } else if (ret["type"] != "undefined") {
      ret["value"] = r.value;
    }
    return ret;
  }
};
/*
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
      auto f = val->getFunctionData();
      wrap = matjson::makeObject({
        {"name", f->name},
        {"line", f->line},
        {"args", f->getArgumentsString()}
      });
    } else {
      matjson::Value obj;
      auto props = val->Childs;
      for (int i = 0; i < val->getChildren(); i++) {
        auto p = props[i];
        obj[p->getName()] = jsvalToJsonVal(p->getVarPtr().getVar());
      }
    }
    ret = matjson::makeObject({
      {"type", type},
      {"value", wrap}
    });
  }
  return ret;
}
*/
$domainMethod(evaluate) {
  auto s = getState();
  auto ret = s->evaluateComplex(params["expression"].asString().unwrapOr("").c_str());
  return geode::Ok(RemoteObject(ret->getVarPtr().getVar()));
}

$execute {
  auto p = Protocol::get();
  p->registerFunction("Runtime.evaluate", &evaluate, {"expression"});
}
