#include "WS.hpp"
//#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocketServer.h>
#include <matjson.hpp>
#include "Geode/ui/Notification.hpp"

std::shared_ptr<Protocol> prot = nullptr;
/// theres only one protocol instance for the entire lifetime so
ix::WebSocketServer* ws;

bool Protocol::init() {
  ix::initNetSystem();
  g = geode::utils::file::readString(geode::Mod::get()->getResourcesDir() / "protocols.json").unwrap();
  ws = new ix::WebSocketServer(1412,"127.0.0.1");

  ws->setOnClientMessageCallback([this](std::shared_ptr<ix::ConnectionState> s, ix::WebSocket& c, const ix::WebSocketMessagePtr& msg){
    if (msg->type == ix::WebSocketMessageType::Open) {
      geode::log::info("new connection chat");
      idsForClient[s->getId()] = std::make_pair(-1, std::vector<int>{});
    }
    else if (msg->type == ix::WebSocketMessageType::Close) {
      geode::log::info("new disconnection chat");
      idsForClient.erase(s->getId());
    }
    else if (msg->type == ix::WebSocketMessageType::Message || !msg->str.empty()) {
      auto p = matjson::parse(msg->str);
      if (p.isErr()) {
        c.sendText(errorResponseStr(-1,-32602,"Malformed input"));
        return;
      }
      auto j = p.unwrap();

      int id;
      auto idOptional = j["id"].asInt();
      if (idOptional.isOk()) id = idOptional.unwrap();
      else {
        c.sendText(errorResponseStr(-1, -32602, "Request ID must be an int."));
        return;
      }
      auto& currentIdInfo = idsForClient[s->getId()];
      int lastId = currentIdInfo.first;
      auto& unusedIds = currentIdInfo.second;
      auto mmm = std::find(unusedIds.begin(), unusedIds.end(), id);
      if (mmm == unusedIds.end() && id <= lastId) {
        c.sendText(errorResponseStr(id, -32602, "ID already used."));
        return;
      }
      if (mmm != unusedIds.end()) {
        unusedIds.erase(mmm);
      } else {
        for (int i = lastId; i < id; i++) {
          unusedIds.push_back(i);
        }
      }
      currentIdInfo.first = id;
      
      auto methodName = j["method"].asString().unwrap();
      if (methodName == "getProtocolInformation") {
        c.sendText(successResponseStr(id, g));
        return;
      }
      decltype(functions)::value_type::second_type method;
      auto i = functions.find(methodName);
      if (i==functions.end()) {
        c.sendText(errorResponseStr(id,-32601,"'"+methodName+"' wasn't found."));
        return;
      }
      auto params = j["params"];
      geode::log::debug("Parameters for {}\n{}", methodName, params.dump(2));
      for (auto& p : i->second.second) {
        if (!params.contains(std::string(p))) {
          c.sendText(errorResponseStr(id,-32602,"Required parameter '"+p+"' not present."));
          return;
        }
      }
      //FunctionReturnType ret = i->second.first(params);
      auto baaa = [this, id, &c](FunctionReturnType* ret) {
      if (ret->isErr()) {
        auto err = ret->unwrapErr();
        c.sendText(errorResponseStr(id, std::get<0>(err), std::get<1>(err)));
        return;
      }
      c.sendText(successResponseStr(id, ret->unwrap()));
      return;
      };

      auto func = i->second.first;
      if (std::holds_alternative<ProtocolSyncFunction>(func)) {
        auto mm = std::get<ProtocolSyncFunction>(func)(params);
        baaa(&mm);
      } else {
        auto g = std::get<ProtocolAsyncFunction>(func);
        // the horror to allocate the task to somewhere else
        // (i hope it works)
        auto task = AsyncFunctionTask::runWithCallback([g,params,&baaa](AsyncFunctionTask::PostResult finish, auto prog, auto cancelled){
          matjson::Value bro = params;
          g(bro, finish);
        });
        AsyncFunctionTask* taskPtr = new AsyncFunctionTask(std::move(task));
        auto listener = new geode::EventListener<AsyncFunctionTask>;
        listener->bind([&baaa, taskPtr, listener](AsyncFunctionTask::Event* e){
          if (auto result = e->getValue()) {
            baaa(result);
            delete taskPtr;
            delete listener; // troll
          }
        });
        listener->setFilter(*taskPtr);
      }
    }
  });
  ws->disablePerMessageDeflate();
  ws->disablePong();
  ws->start();
  auto r = ws->listen();
  if (!r.first) {
    geode::Notification::create("Cannot open WebSocket server: "+r.second, geode::NotificationIcon::Error)->show();
    return false;
  };
  geode::log::debug("we are living, loving and lying with this one");
  running = true;
  return true;
}

std::string debugPrint(matjson::Value const& v) {
  return v.dump(0);
}

std::string Protocol::successResponseStr(int id, matjson::Value const& resp) {
  return debugPrint(matjson::makeObject({
    {"jsonrpc", "2.0"},
    {"id", id},
    {"result", resp}
  }));
};

std::string Protocol::errorResponseStr(int id, int code, std::string message) {
  auto c = matjson::makeObject({
    {"jsonrpc", "2.0"},
    {"error", matjson::makeObject({
      {"code", code},
      {"message", message}
    })}
  });
  if (id < 0) c["id"] = nullptr;
  else c["id"] = id;
  return debugPrint(c);
};

std::shared_ptr<Protocol> Protocol::get() {
  if (!prot) {
    prot = std::make_shared<Protocol>();
  }
  return prot;
};

void Protocol::broadcastEvent(std::string eventName, matjson::Value const& content) {
  if (ws==nullptr) return;
  auto o = matjson::makeObject({
    {"method", eventName},
    {"params", content}
  }).dump(0);
  for (auto& c : ws->getClients()) {
    c->send(o);
  }
}

void Protocol::registerFunction(
  std::string funcName, 
  ProtocolFunction f,
  decltype(Protocol::functions)::value_type::second_type::second_type requiredParams
) { 
  functions.insert_or_assign(
    funcName, 
    std::make_pair(f, requiredParams)
  );
};


void fireEvent(std::string eventName, matjson::Value const& content) {
  if (prot) {
    prot->broadcastEvent(eventName, content);
  }
  /*
  geode::log::debug("e");
  ProtocolEvent(matjson::makeObject({
    {"method", eventName},
    {"params", content}
  })).post();
  */
};
  
void Protocol::close() {
  if (running) {
    ws->stop();
  }
  running = false;
  ix::uninitNetSystem();
};
