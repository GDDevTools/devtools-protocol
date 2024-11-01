#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <chrono>
#include "../WS.hpp"

void fireNetworkEvent(std::string eventName, matjson::Value const &content) {  fireEvent("Network."+eventName, content);
};

bool domainDisabled = true;

int now() {
  return std::chrono::duration_cast<std::chrono::seconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
}
#define wrSelf geode::utils::web::WebRequest* self

std::map<std::size_t, std::string> responseBody;
std::map<std::size_t, std::string> requestPostData;
std::map< 
  std::size_t,
  geode::EventListener<geode::utils::web::WebTask> 
> listeners;
geode::utils::web::WebTask send(wrSelf, std::string_view method, std::string_view url) { 
  if (method=="POST") {
    auto d = self->getBody();
    if (d.has_value())
      requestPostData[self->getID()] = std::string(d->begin(), d->end());
  }
  if (!domainDisabled) {
    fireNetworkEvent("requestWillBeSent",matjson::Object{
      {"requestId", self->getID()},
      {"request", matjson::Object{
        {"url", std::string(url)},
        {"method", std::string(method)},
        {"headers", self->getHeaders()},
        {"postData", self->getBody()}
      }},
      {"timestamp", now()}
    });
  }
  auto task = self->send(method,url);
  ///TODO: Doing this will make EVERY requests run regardless of whether or not a WebTask is listened by the caller
  listeners[self->getID()].bind([self,url](geode::utils::web::WebTask::Event* e){
    if (auto resp = e->getValue()) {
    responseBody[self->getID()] = resp->string().unwrapOr("");
      if (!domainDisabled) {
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
  listeners[self->getID()].setFilter(task);

  return task;
}

$domainMethod(disable) {
  domainDisabled = true;
  return geode::Ok(matjson::Object{});
}
$domainMethod(enable) {
  domainDisabled = false;
  return geode::Ok(matjson::Object{});
}
$execute {
  auto p = Protocol::get();
  p->registerFunction("Network.disable", &disable);
  p->registerFunction("Network.enable", &enable);
}
