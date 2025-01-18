#include <Geode/Geode.hpp>
#include "popups.hpp"

class BezierEasing : public cocos2d::CCActionInterval {
  cocos2d::CCPoint one; cocos2d::CCPoint two;
  cocos2d::CCPoint getBezierPoint(float t) {
    cocos2d::CCPoint tmp[4] {{0,0},one,two,{1,1}};
    int i = 3;
    while (i > 0) {
        for (int k = 0; k < i; k++)
            tmp[k] = tmp[k] + (tmp[k+1] - tmp[k]) * t;
        i--;
    }
    return tmp[0];
  }

  cocos2d::CCActionInterval* action = nullptr;
public:
  bool initWithBezierPoints(cocos2d::CCActionInterval* action, cocos2d::CCPoint o, cocos2d::CCPoint t) {
    if (!cocos2d::CCActionInterval::initWithDuration(action->getDuration())) return false;
    this->action = action;
    action->retain();
    one = o;
    two = t;
    return true;
  }
  ~BezierEasing() {
    action->release();
  }
  static BezierEasing* create(cocos2d::CCActionInterval* action, float x1, float y1, float x2, float y2) {
    auto j = new BezierEasing(); // only works on windows
    if (j && j->initWithBezierPoints(action, {x1, y1}, {x2, y2})) {
      j->autorelease();
    } else {
      delete j;
      j = nullptr;
    }
    return j;
  }
  cocos2d::CCObject* copyWithZone(cocos2d::CCZone *pZone) override {
    cocos2d::CCZone* pNewZone = NULL;
    BezierEasing* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)  {
      //in case of being called at sub class
      pCopy = (BezierEasing*)(pZone->m_pCopyObject);
    }
    else {
      pCopy = new BezierEasing();
      pZone = pNewZone = new cocos2d::CCZone(pCopy);
    }

    
    CCActionInterval::copyWithZone(pZone);

    CC_SAFE_DELETE(pNewZone);

    pCopy->initWithBezierPoints((cocos2d::CCActionInterval*)action->copy()->autorelease(),one,two);

    return pCopy;
  }
  void startWithTarget(cocos2d::CCNode* target) override {
    cocos2d::CCActionInterval::startWithTarget(target);
    action->startWithTarget(target);
  }
  void update(float dd) override {
    if (action == nullptr) return;
    action->update(getBezierPoint(dd).y);
  }
};


#include <Geode/modify/MenuLayer.hpp>
struct amongus : public geode::Modify<amongus, MenuLayer> {
  bool init() {
    if (!MenuLayer::init()) {
      return false;
    }

    auto myButton = CCMenuItemSpriteExtra::create(
      cocos2d::CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
      this,
      menu_selector(amongus::onPlaygroundButton)
    );

    auto menu = this->getChildByID("bottom-menu");
    menu->addChild(myButton);
    menu->updateLayout();

    return true;
  }

  void onPlaygroundButton(CCObject*) {
    PlaygroundPopup::create()->show();
  }
};
