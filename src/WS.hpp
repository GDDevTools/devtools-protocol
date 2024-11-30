
#ifndef __fuck_you__
#define __fuck_you__

#include <Geode/Result.hpp>
#include <Geode/loader/Dispatch.hpp>
#include <matjson.hpp>
using matjsonObjectInitType = std::initializer_list<std::pair<std::string, matjson::Value>>;

class Protocol {
public:
  using FunctionReturnType = geode::Result<
    matjson::Value, 
    std::tuple<int, std::string>
  >;
  bool running = false;
private:
  std::vector<int> usedIds;
  std::unordered_map<
    std::string, // method name
    std::pair<
      std::function<FunctionReturnType(matjson::Value&)>, // function
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
    decltype(functions)::value_type::second_type::first_type function,
    decltype(functions)::value_type::second_type::second_type requiredParams = {}
  );
  ~Protocol() {close();};
  void close();
};
void fireEvent(std::string eventName, matjson::Value const& content);

#define $domainMethod(method) Protocol::FunctionReturnType method(matjson::Value& params)
#define $domainAsyncMethod(method) void method(matjson::Value& params, void(Protocol::FunctionReturnType) finish)

namespace errors {
  inline Protocol::FunctionReturnType invalidParameter(std::string msg) {
    return geode::Err(
      std::make_tuple(-32602, msg)
    );
  }
  inline Protocol::FunctionReturnType requiredParameter(std::string param) {
    return invalidParameter("Required parameter '"+param+"' not present.");
  }
  inline Protocol::FunctionReturnType internalError(std::string msg) {
    return geode::Err(
      std::make_tuple(-32603, msg)
    );
  }
}

#endif // __fuck_you__
