#include "WS.hpp"
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocketMessageType.h>
#include <matjson.hpp>
#include <memory>

std::shared_ptr<Protocol> prot = nullptr;

bool Protocol::init() {
  ix::initNetSystem();
  ws = std::make_unique<ix::WebSocketServer>(1412);

  ws->setOnClientMessageCallback([this](std::shared_ptr<ix::ConnectionState> s, ix::WebSocket& c, const ix::WebSocketMessagePtr& msg){
    if (msg->type == ix::WebSocketMessageType::Message) {
      auto j = matjson::parse(msg->str);
      if (j["id"].is_null()) {
        c.sendText(errorResponseStr(-1,400,"Requests must have an ID."));
        return;
      } 
      int id = j["id"].as_int();
      int usedIdsSize = usedIds.size();
      if (usedIdsSize%50==0) {
        usedIds.reserve(usedIdsSize+50);
      }
      usedIds.push_back(id);
      auto methodName = j["method"].as_string();
      decltype(functions)::value_type::second_type method;
      auto i = functions.find(methodName);
      if (i==functions.end()) {
        c.sendText(errorResponseStr(id,400,"Method "+methodName+" does not exist."));
        return;
      }
      FunctionReturnType ret = i->second(j["params"].as_object());
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
  ws->listenAndStart();
  running = true;
  return true;
}

std::string Protocol::successResponseStr(int id, matjson::Value const& resp) {
  return matjson::Value(matjson::Object{
    {"id", id},
    {"result", resp}
  }).dump(0);
};

std::string Protocol::errorResponseStr(int id, int code, std::string message) {
  return matjson::Value(matjson::Object{
    {"id", id},
    {"error", matjson::Object{
      {"code", code},
      {"message", message}
    }}
  }).dump(0);
};

std::shared_ptr<Protocol> Protocol::get() {
  if (!prot) {
    prot = std::make_shared<Protocol>();
  }
  return prot;
};

void Protocol::broadcastEvent(std::string eventName, matjson::Value const& content) {
  for (auto& c : ws->getClients()) {
    c->send(matjson::Value(matjson::Object{
      {"method", eventName},
      {"params", content}
    }).dump(0));
  }
}

void Protocol::registerFunction(std::string funcName, decltype(Protocol::functions)::value_type::second_type f) { 
  functions.insert_or_assign(funcName, f);
};

void fireEvent(std::string eventName, matjson::Object const &content) {
  if (prot) {
    prot->broadcastEvent(eventName, content);
  }
};
  
void Protocol::close() {
  if (running) {
    for (auto& c : ws->getClients()) {
      c->close(1, "See you next time!");
    }
    ws->stop();
  }
  running = false;
  ix::uninitNetSystem();
};
