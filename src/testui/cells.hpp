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
protected:
  cocos2d::CCNode* m_trailingNode;
  bool init(std::string title, std::string description = "") {
    m_ab = cocos2d::CCLayerColor::create({0,0,0,0});
    addChild(m_ab);

    auto t = cocos2d::CCLabelBMFont::create(title.c_str(), "bigFont.fnt");
    addChild(t);
    t->setPositionX(16);
    t->setPositionY(15);
    t->setAnchorPoint({0,0.5});
    t->setScale(0.5);

    addChild(m_trailingNode);
    m_trailingNode->setPositionX(16);
    m_trailingNode->setPositionY(15);
    m_trailingNode->ignoreAnchorPointForPosition(false);
    m_trailingNode->setAnchorPoint({0,0.5});
    m_trailingNode->setScale(0.5);

    return true;
  }
public:
  void setBGOpacity(GLubyte idk) {
    m_ab->setOpacity(idk);
  }
  void setContentSize(cocos2d::CCSize const& size) override {
    cocos2d::CCNode::setContentSize(size);
    m_ab->setContentSize(size);
    m_trailingNode->setPositionX(size.width - 16*2);
  }
};

class NavigationCell : public stupidcell {
protected:
  bool init(std::string title, std::string description = "") {
    auto bubu = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    bubu->setFlipX(true);
    auto bub = CCMenuItemSpriteExtra::create(bubu, this, menu_selector(NavigationCell::mf));
    bub->setPosition(bubu->getContentSize()/2);
    m_trailingNode = cocos2d::CCMenu::create();
    m_trailingNode->setContentSize(bub->getContentSize());
    m_trailingNode->addChild(bub);

    return stupidcell::init(title, description);
  }
  
  virtual void onNavigateClick() {}
  void mf(cocos2d::CCObject*) {
    onNavigateClick();
  }
};

class DomainCell : public NavigationCell {
  Domain domainInfo;
  bool init(Domain& domain) {
    NavigationCell::init(domain.domain);
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

class MethodCell : public NavigationCell {
  Method methodInfo;
  bool init(Method& method) {
    NavigationCell::init(method.name);
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
