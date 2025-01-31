
#ifndef __fuck_you__
#define __fuck_you__

#include <Geode/Result.hpp>
#include <Geode/loader/Dispatch.hpp>
#include <matjson.hpp>
#include <variant>
using matjsonObjectInitType = std::initializer_list<std::pair<std::string, matjson::Value>>;

struct ClientConfig {
  bool isDomainEnabled(std::string domain);
  void setDomainEnabled(std::string domain, bool enable);
private:
  std::string id;

  ClientConfig(std::string id) : id(id) {}
  friend class Protocol;
};

class Protocol {
  std::string g;
  std::vector<std::string> domainsList;
  friend struct ClientConfig;
public:
  using FunctionErrorReturnType = std::pair<int, std::string>;
  using FunctionReturnType = geode::Result<
    matjson::Value, 
    FunctionErrorReturnType  
  >;
  using AsyncFunctionTask = geode::Task<FunctionReturnType>;
  bool running = false;
  using ProtocolSyncFunction = std::function<FunctionReturnType(ClientConfig, matjson::Value&)>;
  using ProtocolAsyncFunction = std::function<void(ClientConfig, matjson::Value&, AsyncFunctionTask::PostResult)>;
  using ProtocolFunction = std::variant<
    ProtocolSyncFunction,
    ProtocolAsyncFunction
  >;
private:
  std::unordered_map<
    std::string, // client id
    std::pair<
      int, // last id
      std::vector<int> // unused id
    >
  > idsForClient;

  std::unordered_map<
    std::string, // domain name
    std::unordered_map<
      std::string, // client id
      bool // domain enabled
    >
  > domainEnabledState;

  std::unordered_map<
    std::string, // method name
    std::pair<
      ProtocolFunction, // function
      std::vector< // required parameters
        std::string // name
      >
    >
  > functions;

  std::string successResponseStr(int id, matjson::Value const& resp);
  std::string errorResponseStr(int id, int code, std::string message);
public:
  bool init();
  static std::shared_ptr<Protocol> get();

  void broadcastEvent(std::string eventName, matjson::Value const& content);
  void registerFunction(
    std::string funcName, 
    ProtocolFunction function,
    decltype(functions)::value_type::second_type::second_type requiredParams = {}
  );
  ~Protocol() {close();};
  void close();
};

#define $fireEventArguments std::string eventName, matjson::Value const& content = {}
void fireEvent($fireEventArguments);

inline Protocol::FunctionReturnType emptyResponse() {
  return geode::Ok(matjson::Value::object());
}

#define $domainMethod(method) static Protocol::FunctionReturnType method( \
  ClientConfig info, \
  matjson::Value& params \
)
#define $domainMethodUndepended(method) static Protocol::FunctionReturnType method( \
  matjson::Value& params \
)
#define $domainAsyncMethod(method) static void method( \
  ClientConfig info, \
  matjson::Value& params, \
  Protocol::AsyncFunctionTask::PostResult finish \
)
#define $domainAsyncMethodUndepended(method) static void method( \
  matjson::Value& params, \
  Protocol::AsyncFunctionTask::PostResult finish \
)

namespace errors {
  inline Protocol::FunctionReturnType invalidParameter(std::string msg) {
    return geode::Err(
      std::make_pair(-32602, msg)
    );
  }
  inline Protocol::FunctionReturnType requiredParameter(std::string param) {
    return invalidParameter("Required parameter '"+param+"' not present.");
  }
  inline Protocol::FunctionReturnType internalError(std::string msg) {
    return geode::Err(
      std::make_pair(-32603, msg)
    );
  }
  inline Protocol::FunctionReturnType userInterventionError(std::string msg) {
    return geode::Err(
      std::make_pair(-32001, msg)
    );
  }
}

#endif // __fuck_you__
