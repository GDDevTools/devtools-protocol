#pragma once
#include "Geode/cocos/cocoa/CCObject.h"
#include "protocols_json_struct.hpp"

class TitleCell : public cocos2d::CCNode {
  cocos2d::CCLabelBMFont* m_nameLabel;
  CCMenuItemToggler* m_collapseToggle;
  cocos2d::CCMenu* m_buttonMenu;
  bool init(std::string title);
  void oniowgoiweogi(cocos2d::CCObject *);

public:
  bool isCollapsed() {return !m_collapseToggle->isToggled();}
  void setContentSize(cocos2d::CCSize const& size) override {
    cocos2d::CCNode::setContentSize(size);
    m_buttonMenu->setPosition({size.width-16, size.height/2});
  }
  static TitleCell* create(std::string title) {
    auto m = new TitleCell();
    if (m->init(title)) {
      m->autorelease();
    } else {
      delete m;
      m = nullptr;
    }
    return m;
  }

};

/// The second wheel has benn reimplemented
class stupidcell : public cocos2d::CCNode {
  cocos2d::CCLayerColor* m_ab;
  cocos2d::CCMenu* button;
protected:
  bool init(std::string title, std::string description = "") {
    m_ab = cocos2d::CCLayerColor::create({0,0,0,0});
    addChild(m_ab);

    auto t = cocos2d::CCLabelBMFont::create(title.c_str(), "bigFont.fnt");
    addChild(t);
    t->setPositionX(16);
    t->setPositionY(15);
    t->setAnchorPoint({0,0.5});
    t->setScale(0.5);

    auto bubu = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    bubu->setFlipX(true);
    auto bub = CCMenuItemSpriteExtra::create(bubu, this, menu_selector(stupidcell::mf));
    bub->setPosition(bubu->getContentSize()/2);
    button = cocos2d::CCMenu::create();
    button->addChild(bub);
    addChild(button);
    button->setPositionX(16);
    button->setPositionY(15);
    button->setContentSize(bub->getContentSize());
    button->ignoreAnchorPointForPosition(false);
    button->setAnchorPoint({0,0.5});
    button->setScale(0.5);

    return true;
  }
  virtual void onNavigateClick() {}
  void mf(cocos2d::CCObject*) {
    onNavigateClick();
  }
public:
  void setBGOpacity(GLubyte idk) {
    m_ab->setOpacity(idk);
  }
  void setContentSize(cocos2d::CCSize const& size) override {
    cocos2d::CCNode::setContentSize(size);
    m_ab->setContentSize(size);
    button->setPositionX(size.width - 16*2);
  }
  
};

class DomainCell : public stupidcell {
  Domain domainInfo;
  bool init(Domain& domain) {
    stupidcell::init(domain.domain);
    domainInfo = domain;
    
    setContentSize({150,30}); // height must be 30 or else
    return true;
  }
  void onNavigateClick() override;

public:
  static DomainCell* create(Domain& info) {
    auto j = new DomainCell();
    if (j && j->init(info)) {
      j->autorelease();
    } else {
      delete j;
      j = nullptr;
    }
    return j;
  }
};

class MethodCell : public stupidcell {
  Method methodInfo;
  bool init(Method& method) {
    stupidcell::init(method.name);
    methodInfo = method;

    setContentSize({150,30}); // height must be 30 or else
    return true;
  }
  void onNavigateClick() override;

public:
  static MethodCell* create(Method& info) {
    auto j = new MethodCell();
    if (j && j->init(info)) {
      j->autorelease();
    } else {
      delete j;
      j = nullptr;
    }
    return j;
  }
};
