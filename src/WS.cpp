#include "WS.hpp"
#include "Geode/loader/Loader.hpp"
//#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocketServer.h>
#include <matjson.hpp>
#include "stuff.hpp"

std::shared_ptr<Protocol> prot = nullptr;
/// theres only one protocol instance for the entire lifetime so
ix::WebSocketServer* ws;

bool Protocol::init() {
  ix::initNetSystem();
  ws = new ix::WebSocketServer(1412,"127.0.0.1");

  ws->setOnClientMessageCallback([this](std::shared_ptr<ix::ConnectionState> s, ix::WebSocket& c, const ix::WebSocketMessagePtr& msg){
    
    if (msg->type == ix::WebSocketMessageType::Open) {
      geode::log::info("new connection chat");
    }
    else if (msg->type == ix::WebSocketMessageType::Message || !msg->str.empty()) {
      auto p = matjson::parse(msg->str);
      if (p.isErr()) {
        c.sendText(errorResponseStr(-1,-32602,"Malformed input"));
        return;
      }
      auto j = p.unwrap();
      if (!j["id"].isNumber()) {
        c.sendText(errorResponseStr(-1,-32602,"Request ID must be a number."));
        return;
      } 
      int id;
      auto idOptional = j["id"].asInt();
      if (idOptional.isOk()) id = idOptional.unwrap();
      else {
        c.sendText(errorResponseStr(-1, -32602, "Invalid 'id' type."));
        return;
      }
      if (std::find(usedIds.begin(), usedIds.end(), id) != usedIds.end()) {
        c.sendText(errorResponseStr(id, -32602, "ID already used."));
        return;
      }
      int usedIdsSize = usedIds.size();
      if (usedIdsSize%50==0) {
        usedIds.reserve(usedIdsSize+50);
      }
      usedIds.push_back(id);
      auto methodName = j["method"].asString().unwrap();
      decltype(functions)::value_type::second_type method;
      auto i = functions.find(methodName);
      if (i==functions.end()) {
        c.sendText(errorResponseStr(id,-32601,"'"+methodName+"' wasn't found."));
        return;
      }
      auto params = j["params"];
      for (auto& p : i->second.second) {
        if (!params.contains(std::string(p))) {
          c.sendText(errorResponseStr(id,-32602,"Required parameter '"+p+"' not present."));
          return;
        }
      }
      FunctionReturnType ret = i->second.first(params);
      if (ret.isErr()) {
        auto err = ret.unwrapErr();
        c.sendText(errorResponseStr(id, std::get<0>(err), std::get<1>(err)));
        return;
      }
      c.sendText(successResponseStr(id, ret.unwrap()));
      return;
    }
  });
  ws->disablePerMessageDeflate();
  ws->disablePong();
  ws->start();
  auto r = ws->listen();
  if (!r.first) {throw std::runtime_error(r.second);};
  geode::log::debug("we are living, loving and lying with this one");
  running = true;
  return true;
}

std::string Protocol::successResponseStr(int id, matjson::Value const& resp) {
  return matjson::makeObject({
    {"id", id},
    {"result", resp}
  }).dump(0);
};

std::string Protocol::errorResponseStr(int id, int code, std::string message) {
  return matjson::makeObject({
    {"id", id},
    {"error", matjson::makeObject({
      {"code", code},
      {"message", message}
    })}
  }).dump(0);
};

std::shared_ptr<Protocol> Protocol::get() {
  if (!prot) {
    prot = std::make_shared<Protocol>();
  }
  return prot;
};

void Protocol::broadcastEvent(std::string eventName, matjson::Value const& content) {
  if (ws==nullptr) return;
  for (auto& c : ws->getClients()) {
    c->send(matjson::makeObject({
      {"method", eventName},
      {"params", content}
    }).dump(0));
  }
}

void Protocol::registerFunction(
  std::string funcName, 
  decltype(Protocol::functions)::value_type::second_type::first_type f,
  decltype(Protocol::functions)::value_type::second_type::second_type requiredParams
) { 
  functions.insert_or_assign(funcName, std::make_pair(f,requiredParams));
};

void fireEvent(std::string eventName, matjson::Value const& content) {
  if (prot) {
    prot->broadcastEvent(eventName, content);
  }
};
  
void Protocol::close() {
  if (running) {
    ws->stop();
  }
  running = false;
  ix::uninitNetSystem();
};
