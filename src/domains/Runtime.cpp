// ?

#include "Geode/DefaultInclude.hpp"
#include "../WS.hpp"
#include <fmt/format.h>
#include <matjson.hpp>
#include "Geode/loader/Loader.hpp"
#include "external/tinyjs/TinyJS.hpp"
#include "jsenv/console.hpp"
#include "jsenv/state.hpp"
#undef inline

std::string descriptionFor(CScriptVarLink* link) {
  CScriptVar* var = link->getVarPtr().getVar();
  if (var->isObject()) {
    CScriptResult amogus;
    auto g = 
      var->callJS_toString(amogus);
    return g->toString();
  }
  return "";
}

matjson::Value serializePreview(CScriptVar* val) {
  std::string type;
  std::string subtype;
  std::string className;

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
    else if (val->isAccessor())  subtype = "accessor";
  }

  auto ret = matjson::makeObject( {
    {"type", type}
  });
  if (!subtype.empty()) ret.set("subtype", subtype);

  return ret;
}

matjson::Value serializeObjectPreview(CScriptVarLink* val);
matjson::Value serializePropertyPreview(CScriptVarLink* val) {
  /*
  
  */
  auto ret = serializePreview(val->getVarPtr().getVar());
  ret["name"] = val->getName();
  // check if subtype exists before comparing or else it will create a new object under that property
  if (val->getVarPtr()->isObject()) {
    if (
      !ret.contains("subtype")  // doesnt have a subtype hint (custom class or just object)
      //ret["subtype"].asString().unwrapOr("").empty()
      ||                            // or
      val->getVarPtr()->isArray()   // does have a subtype hint and is array (for now)
    )
    ret["valuePreview"] = serializeObjectPreview(val);
  }

  return ret;
}

matjson::Value serializeObjectPreview(CScriptVarLink* link) {
  auto val = link->getVarPtr().getVar();
  auto ret = serializePreview(val);
  if (val->isObject() && !val->isFunction()) {
    std::vector<matjson::Value> props;
    props.reserve(val->Childs.size());
    for (auto& child : val->Childs) {
      CScriptVarLink* link = child.operator->();
      // prevent recursion
      if (link->getVarPtr().getVar() != val)
        props.push_back(serializePropertyPreview(link));
    }
    ret["properties"] = props;
  }
  auto desc = descriptionFor(link);
  if (!desc.empty()) ret["description"] = desc;

  return ret;
}

matjson::Value serializeRemoteObject(CScriptVarLink* val) {
  matjson::Value value;
  matjson::Value preview;
  auto ret = serializePreview(val->getVarPtr().getVar());
  std::string type = ret["type"].asString().unwrap();
  preview = serializeObjectPreview(val);
  if (false) {}
  else if (type=="object") {
  }
  else if (type=="string") value = val->toString();
  else if (type=="boolean") value = val->toBoolean();
  else if (type=="number") value = val->toNumber().toDouble();
  ret["preview"] = preview;
  if (type == "object") {
  } else if (type != "undefined") {
    ret["value"] = value;
  }
  return ret;
}
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

std::string error_type_enum_name[6] = {
  "Error",
	"EvalError",
	"RangeError",
	"ReferenceError",
	"SyntaxError",
	"TypeError"
};
$domainAsyncMethod(evaluate) {
  geode::queueInMainThread([params, finish]{
    auto s = getState();
    #ifdef GEODE_IS_WINDOWS
    if (IsDebuggerPresent()) DebugBreak();
    #endif
    try {
      auto ret = s->evaluateComplex(params["expression"].asString().unwrapOr("").c_str());
      finish(geode::Ok(serializeRemoteObject(ret.getVar())));
    } catch (CScriptException* e) {
      geode::log::logImpl(geode::Severity::Error, theFakeJSMod(), "[JavaScript]: {}: {}", error_type_enum_name[e->errorType], e->message);
      finish(errors::internalError(fmt::format("{}: {}", error_type_enum_name[e->errorType], e->message)));
    }
  });
}

$execute {
  auto p = Protocol::get();
  p->registerFunction("Runtime.evaluate", &evaluate, {"expression"});
}
