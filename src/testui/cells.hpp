#pragma once
#include <Geode/Geode.hpp>
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
  cocos2d::CCLabelBMFont* m_title;
  cocos2d::CCLabelBMFont* m_description = nullptr;
  cocos2d::CCNode* m_trailingNode;
  virtual bool init(std::string title, std::string description = "");

public:
  void setBGOpacity(GLubyte idk) {
    m_ab->setOpacity(idk);
  }
  void setContentSize(cocos2d::CCSize const& size) override {
    cocos2d::CCNode::setContentSize(size);
    float centerY = getContentSize().height/2;
    m_title->setPosition({16, centerY + (m_description == nullptr ? 0.f : 6.f)});
    if (m_description) m_description->setPosition({16, centerY-6});
    m_ab->setContentSize(size);
    m_trailingNode->setPosition({size.width - 16, centerY});
  }
};

#pragma region Navigation
class NavigationCell : public stupidcell {
protected:
  bool init(std::string title, std::string description = "") override {
    auto bubu = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    bubu->setFlipX(true);
    bubu->setID("arrow");
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
    NavigationCell::init(domain.domain, domain.description);
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
    NavigationCell::init(method.name, method.description);
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

class EventCell : public NavigationCell {
  Event eventInfo;
  bool init(Event& event) {
    NavigationCell::init(event.name, event.description);
    eventInfo = event;
    static_cast<cocos2d::CCSprite*>(
      m_trailingNode->getChildByIDRecursive("arrow")
    )->setDisplayFrame(
      cocos2d::CCSpriteFrameCache::get()->spriteFrameByName("GJ_arrow_03_001.png")
    );

    setContentSize({150,30}); // height must be 30 or else
    return true;
  }
  void onNavigateClick() override;

public:
  static EventCell* create(Event& info) {
    auto j = new EventCell();
    if (j && j->init(info)) {
      j->autorelease();
    } else {
      delete j;
      j = nullptr;
    }
    return j;
  }
};

class TypeCell : public NavigationCell {
  Type_ typeInfo;
  bool init(Type_& type) {
    NavigationCell::init(type.id, type.description);
    typeInfo = type;
    static_cast<cocos2d::CCSprite*>(
      m_trailingNode->getChildByIDRecursive("arrow")
    )->setDisplayFrame(
      cocos2d::CCSpriteFrameCache::get()->spriteFrameByName("GJ_arrow_02_001.png")
    );

    setContentSize({150,30}); // height must be 30 or else
    return true;
  }
  void onNavigateClick() override {};

public:
  static TypeCell* create(Type_& info) {
    auto j = new TypeCell();
    if (j && j->init(info)) {
      j->autorelease();
    } else {
      delete j;
      j = nullptr;
    }
    return j;
  }
};
#pragma endregion
