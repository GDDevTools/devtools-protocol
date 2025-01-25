#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/cocos/extensions/network/HttpRequest.h>
#include "../WS.hpp"
#include "../stuff.hpp"


void fireNetworkEvent(std::string eventName, matjson::Value const &content) {  
  fireEvent("Network."+eventName, content);
};

bool NetworkDomainDisabled = true;

#define wrSelf geode::utils::web::WebRequest* self

std::map<std::size_t, std::string> responseBody;
std::map<std::size_t, std::string> requestPostData;

#pragma region Geode WebRequest
std::vector<std::tuple<std::size_t, std::string, geode::utils::web::WebTask*>> tasks;
decltype(tasks) removeQueue;
/// a class that listens to WebTask without actually listening to it
/// (so the task can immediately cancels if theres no one else listening to it)
class WebTasksListener : public CCObject {
  void update(float d) override {
    removeQueue.reserve(tasks.size());
    for (auto& p : tasks) {
      auto task = get<2>(p);
      auto id = get<0>(p);
      auto url = get<1>(p);
      if (task->isCancelled()) {
        removeQueue.push_back(p);
      } else if (task->isFinished()) {
        auto resp = task->getFinishedValue();
        fireNetworkEvent("responseReceived", matjson::makeObject({
          {"requestId", id},
          {"response", matjson::makeObject({
            {"url",url},
            {"status",resp->code()},
            {"headers",resp->headers()},
            {"responseTime", now()}
          })},
          {"timestamp", now()}
        }));
        removeQueue.push_back(p);
      }
    }
    for (auto& p : removeQueue) {
      //TODO: e
      tasks.erase(std::find(tasks.begin(), tasks.end(), p));
    }
  }
};
geode::utils::web::WebTask send(wrSelf, std::string_view method, std::string_view url) { 
  if (method=="POST") {
    auto d = self->getBody();
    if (d.has_value())
      requestPostData[self->getID()] = std::string(d->begin(), d->end());
  }
  auto sUrl = std::string(url);
  if (!NetworkDomainDisabled) {
    fireNetworkEvent("requestWillBeSent",matjson::makeObject({
      {"requestId", self->getID()},
      {"initiator", "geode"},
      {"request", matjson::makeObject({
        {"url", sUrl},
        {"method", std::string(method)},
        {"headers", self->getHeaders()},
        {"postData", self->getBody()}
      })},
      {"timestamp", now()}
    }));
  }
  auto task = self->send(method,url);
  tasks.push_back(std::make_tuple(self->getID(),sUrl,&task));
  return task;
}
geode::Hook* wrSendHook;
#pragma endregion

/*
// this abomination is untested
#pragma region Cocos2d HttpClient

// There might be like 5 response handler function in gd but we have to 
// take into account for some insane individual not using geode web
#ifdef GEODE_IS_WINDOWS
  #define extensions "libExtensions.dll"
  #define dispatchRespOrig_addr 0x17fb0
#elif defined(GEODE_IS_ANDROID)
  #define extensions "libcocos2dcpp.so"
  #define dispatchRespOrig_addr 0xa6e148
#endif


static int lastId = 0;
static uintptr_t base = getModule(extensions);

geode::cocos::CCArrayExt<cocos2d::CCArray*> reqids;

void dispatchResponseCallbacks(cocos2d::extension::CCHttpClient* self, float d) {
  auto responseQueue = reinterpret_cast<cocos2d::CCArray*>(
    base +
  #if defined(GEODE_IS_WINDOWS)
    0x465a8
  #elif defined(GEODE_IS_ANDROID)
    0x00aae298
  #else
    0
  #endif
  );
  if (responseQueue->count()) {
    auto resp = static_cast<cocos2d::extension::CCHttpResponse*>(responseQueue->objectAtIndex(0));
    int id = 0;
    bool done = false;
    for (auto& p : reqids) {
      if (static_cast<cocos2d::extension::CCHttpRequest*>(p->lastObject()) == resp->getHttpRequest()) {
        id = static_cast<cocos2d::CCInteger*>(p->firstObject())->getValue();
        done = true;
        break;
      }
    }
    if (!done) throw std::runtime_error("huh? theres no request in the list? (if its known to happen please make this a log)");
    if (!NetworkDomainDisabled) {
      auto respD = resp->getResponseData();
      responseBody[id] = std::string(respD->begin(), respD->end());
      matjson::Object headers;
      std::string line;
      std::stringstream ss(std::string(resp->getResponseHeader()->begin(),resp->getResponseHeader()->end()));
      while (std::getline(ss, line)) {
        auto colon = line.find(':');
        if (colon == std::string::npos) continue;
        auto key = line.substr(0, colon);
        auto value = line.substr(colon + 2);
        if (value.ends_with('\r')) {
          value = value.substr(0, value.size() - 1);
        }
        headers[key] = value;
      }
      fireNetworkEvent("responseReceived", matjson::Object{
        {"requestId", id},
        {"response", matjson::Object{
          {"url",resp->getHttpRequest()->getUrl()},
          {"status",resp->getResponseCode()},
          {"headers",headers},
          {"responseTime", now()}
        }},
        {"timestamp", now()}
      });
    }
  }
  // my source is https://stackoverflow.com/a/50888408 so dont blame me
  reinterpret_cast<void(*)(cocos2d::extension::CCHttpClient*, float)>(
    reinterpret_cast<uintptr_t>(self) 
    +
    dispatchRespOrig_addr // the func offset
  )(self, d);
}
std::string httpTypeToString(cocos2d::extension::CCHttpRequest::HttpRequestType t) {
  switch (t) {
    case cocos2d::extension::CCHttpRequest::HttpRequestType::kHttpGet: return "GET";
    case cocos2d::extension::CCHttpRequest::HttpRequestType::kHttpPost: return "POST";
    case cocos2d::extension::CCHttpRequest::HttpRequestType::kHttpPut: return "PUT";
    case cocos2d::extension::CCHttpRequest::HttpRequestType::kHttpDelete: return "DELETE";
    case cocos2d::extension::CCHttpRequest::HttpRequestType::kHttpUnkown: return "?";
  }
}
#include <Geode/modify/CCHttpClient.hpp>
class $modify(cocos2d::extension::CCHttpClient) {
  void send(cocos2d::extension::CCHttpRequest* request){
    std::string sUrl(request->getUrl());
    auto j = cocos2d::CCArray::create();
    int id = ++lastId;
    j->addObject(CCInteger::create(id));
    j->addObject(request);
    reqids.push_back(j);
    if (!NetworkDomainDisabled) {
      matjson::Object headers;
      if (true) {
        auto headersStr = request->getHeaders();
        for (std::string& h : headersStr) {
          auto seppos = h.find(": ");
          bool cs = true;
          if (seppos == std::string::npos) {
            seppos = h.find(":");
            cs = false;
          }
          if (seppos == std::string::npos) continue;
          headers[h.substr(0,seppos)] = h.substr(seppos + (cs?2:1));
        }
      }

      fireNetworkEvent("requestWillBeSent",matjson::Object{
        {"requestId", id},
        {"initiator", "cocos"},
        {"request", matjson::Object{
          {"url", sUrl},
          {"method", httpTypeToString(request->getRequestType())},
          {"headers", headers},
          {"postData", std::string(request->getRequestData())}
        }},
        {"timestamp", now()}
      });
    }
    cocos2d::extension::CCHttpClient::send(request);
  }
};
geode::Hook* ccDispatchHook;
$execute {
  ccDispatchHook = geode::Mod::get()->hook(
    reinterpret_cast<void*>(base+dispatchRespOrig_addr),
    &dispatchResponseCallbacks,
    "cocos2d::extension::CCHttpClient::dispatchResponseCallbacks",
    tulip::hook::TulipConvention::Thiscall
  ).unwrapOr(nullptr);
}

#pragma endregion

*/

$domainMethod(disableNetwork) {
  NetworkDomainDisabled = true;
  if (wrSendHook) wrSendHook->disable();
  //if (ccDispatchHook) ccDispatchHook->disable();
  return emptyResponse();
}
$domainMethod(enableNetwork) {
  NetworkDomainDisabled = false;
  if (wrSendHook) wrSendHook->enable();
  //if (ccDispatchHook) ccDispatchHook->enable();
  return emptyResponse();
}
$domainMethod(getRequestPostData) {
  auto i = requestPostData.find(params["requestId"].asInt().unwrap());
  if (i!=requestPostData.end()) {
    return geode::Ok(matjson::makeObject({
      {"postData", i->second}
    }));
  }
  return errors::internalError("No request with specified ID or the request does not have a POST data.");
}
$domainMethod(getResponseBody) {
  auto i = responseBody.find(params["requestId"].asInt().unwrap());
  if (i!=responseBody.end()) {
    return geode::Ok(matjson::makeObject({
      {"body", i->second}
    }));
  }
  return errors::internalError("No request with specified ID.");
}
$execute {
  wrSendHook = geode::Mod::get()->hook(
    reinterpret_cast<void*>(
      geode::addresser::getNonVirtual(
        &geode::utils::web::WebRequest::send
      )
    ),
    &send,
    "geode::utils::web::WebRequest::send",
    tulip::hook::TulipConvention::Thiscall
  ).unwrapOr(nullptr);
  if (wrSendHook) wrSendHook->disable();
  auto p = Protocol::get();
  p->registerFunction("Network.disable", &disableNetwork);
  p->registerFunction("Network.enable", &enableNetwork);
  p->registerFunction("Network.getRequestPostData", &getRequestPostData);
  p->registerFunction("Network.getResponseBody", &getResponseBody);
}
