#include "Geode/utils/Result.hpp"
#include <Geode/loader/Dispatch.hpp>
#include <matjson.hpp>
#include <ixwebsocket/IXWebSocketServer.h>

class Protocol {
public:
  using FunctionReturnType = geode::Result<
    matjson::Object, 
    std::tuple<int, std::string>
  >;
  bool running = false;
private:
  std::vector<int> usedIds;
  std::unique_ptr<ix::WebSocketServer> ws;
  std::unordered_map<std::string, std::function<FunctionReturnType(matjson::Object&)>> functions;

  std::string successResponseStr(int id, matjson::Value const& resp);
  std::string errorResponseStr(int id, int code, std::string message);
public:
  bool init();
  static std::shared_ptr<Protocol> get();

  void broadcastEvent(std::string eventName, matjson::Value const& content);
  void registerFunction(std::string funcName, decltype(functions)::value_type::second_type function);
  ~Protocol() {close();};
  void close();
};

void fireEvent(std::string eventName, matjson::Value const &content);
#define $domainMethod(method) Protocol::FunctionReturnType method(matjson::Object& params)
