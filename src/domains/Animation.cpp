#include "../WS.hpp"

void fireAnimEvent(std::string eventName, matjson::Value const &content) {
  fireEvent("Animation."+eventName, content);
}

bool animationDomainDisabled = true;
$domainMethod(disableAnimation) {
  animationDomainDisabled = true;
  return geode::Ok(matjson::Object{});
}
$domainMethod(enableAnimation) {
  animationDomainDisabled = true;
  return geode::Ok(matjson::Object{});
}

std::map<std::unique_ptr<cocos2d::CCAction>, int> ids;
int lastId = 0;

struct Animation {
  int id;
  std::string type;
  bool pausedState;

  Animation(cocos2d::CCAction* a) {
    if (auto intervalAct = geode::cast::typeinfo_cast<cocos2d::CCActionInterval*>(a)) {
      type = "CocosIntervalAction";
      pausedState = intervalAct->getTarget()->getActionManager()->
    }
  }
}

#include <Geode/modify/CCAction.hpp>
class $modify(cocos2d::CCAction) {
  void constructor() {
    cocos2d::CCAction(); // ?
    int id = ++lastId;
    std::unique_ptr<cocos2d::CCAction> e {static_cast<cocos2d::CCAction*>(this)};
    ids[e] = id;
    if (!animationDomainDisabled) {
      fireAnimEvent("animationCreated", matjson::Object{{"id", id}});
    }
  }
  void startWithTarget(cocos2d::CCNode* target) {
    cocos2d::CCAction::startWithTarget(target);
    if (!animationDomainDisabled) {
      fireAnimEvent("animationCreated", matjson::Object{{"id", 0}});
    }
  }
};

$execute {
  auto p = Protocol::get();
  p->registerFunction("Animation.disable",&disableAnimation);
  p->registerFunction("Animation.enable",&enableAnimation);
}
