#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "../WS.hpp"
#include "../stuff.hpp"

void fireNetworkEvent(std::string eventName, matjson::Value const &content) {  
  fireEvent("Network."+eventName, content);
};

bool NetworkDomainDisabled = true;

#define wrSelf geode::utils::web::WebRequest* self

std::map<std::size_t, std::string> responseBody;
std::map<std::size_t, std::string> requestPostData;
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
        fireNetworkEvent("responseReceived", matjson::Object{
          {"requestId", id},
          {"response", matjson::Object{
            {"url",url},
            {"status",resp->code()},
            {"headers",resp->headers()},
            {"responseTime", now()}
          }},
          {"timestamp", now()}
        });
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
    fireNetworkEvent("requestWillBeSent",matjson::Object{
      {"requestId", self->getID()},
      {"request", matjson::Object{
        {"url", sUrl},
        {"method", std::string(method)},
        {"headers", self->getHeaders()},
        {"postData", self->getBody()}
      }},
      {"timestamp", now()}
    });
  }
  auto task = self->send(method,url);
  tasks.push_back(std::make_tuple(self->getID(),sUrl,&task));
  ///TODO: Doing this will make EVERY requests run regardless of whether or not a WebTask is listened by the caller
  /*
  listeners[self->getID()].bind([self,url](geode::utils::web::WebTask::Event* e){
    if (auto resp = e->getValue()) {
    responseBody[self->getID()] = resp->string().unwrapOr("");
      if (!NetworkDomainDisabled) {
        fireNetworkEvent("responseReceived", matjson::Object{
          {"requestId", self->getID()},
          {"response", matjson::Object{
            {"url",std::string(url)},
            {"status",resp->code()},
            {"headers",resp->headers()},
            {"responseTime", now()}
          }},
          {"timestamp", now()}
        });
      }
    }
  });
*/
  return task;
}
geode::Hook* wrSendHook;
$domainMethod(disableNetwork) {
  NetworkDomainDisabled = true;
  if (wrSendHook) wrSendHook->disable();
  return geode::Ok(matjson::Object{});
}
$domainMethod(enableNetwork) {
  NetworkDomainDisabled = false;
  if (wrSendHook) wrSendHook->enable();
  return geode::Ok(matjson::Object{});
}
$domainMethod(getRequestPostData) {
  auto i = requestPostData.find(params["requestId"].as_int());
  if (i!=requestPostData.end()) {
    return geode::Ok(matjson::Object{
      {"postData", i->second}
    });
  }
  return errors::internalError("No request with specified ID or the request does not have a POST data.");
}
$domainMethod(getResponseBody) {
  auto i = responseBody.find(params["requestId"].as_int());
  if (i!=responseBody.end()) {
    return geode::Ok(matjson::Object{
      {"body", i->second}
    });
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
