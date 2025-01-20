#include "cells.hpp"
#include "popups.hpp"

std::string replace(std::string& str, const std::string& from, const std::string& to) {
  size_t s = 0;
  while ((s = str.find(from,s))!=std::string::npos) {
    str.replace(s, from.size(), to);
    s+=to.size();
  }
  return str;
}

bool stupidcell::init(std::string title, std::string description) {
  description = replace(description, "\n", " ");
  m_ab = cocos2d::CCLayerColor::create({0, 0, 0, 0});
  addChild(m_ab);

  m_title = cocos2d::CCLabelBMFont::create(title.c_str(), "bigFont.fnt");
  addChild(m_title);
  m_title->setAnchorPoint({0, 0.5});
  m_title->setScale(0.5);

  if (description.size() != 0) {
    int limit = 80;
    if (description.size() > limit) {
      description = description.substr(0, limit - 3) + "...";
    }
    m_description = cocos2d::CCLabelBMFont::create(
      description.c_str(), "chatFont.fnt"
    );
    addChild(m_description);
    m_description->setAnchorPoint({0, 0.5});
    m_description->setScale(0.45);
    m_description->setColor({170, 169, 169});
  }

  addChild(m_trailingNode);
  m_trailingNode->setPositionX(0);
  m_trailingNode->setPositionY(15);
  m_trailingNode->ignoreAnchorPointForPosition(false);
  m_trailingNode->setAnchorPoint({1, 0.5});
  m_trailingNode->setScale(0.5);

  return true;
}


void DomainCell::onNavigateClick() {
  cocos2d::CCDirector::get()->getRunningScene()->getChildByType<PlaygroundPopup>(0)->navigateToDomainContents(domainInfo);
}
bool TitleCell::init(std::string title) {
  m_buttonMenu = cocos2d::CCMenu::create();

  auto collapseSprBG = cocos2d::CCSprite::create("square02c_001.png");
  collapseSprBG->setColor({25, 25, 25});
  collapseSprBG->setOpacity(105);
  auto collapseSpr =
      cocos2d::CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
  collapseSpr->setScale(1.9f);
  collapseSprBG->addChildAtPosition(collapseSpr, geode::Anchor::Center);
  collapseSprBG->setScale(.2f);

  auto uncollapseSprBG = cocos2d::CCSprite::create("square02c_001.png");
  uncollapseSprBG->setColor({25, 25, 25});
  uncollapseSprBG->setOpacity(105);
  auto uncollapseSpr =
      cocos2d::CCSprite::createWithSpriteFrameName("edit_delCBtn_001.png");
  uncollapseSpr->setScale(1.5f);
  uncollapseSprBG->addChildAtPosition(uncollapseSpr, geode::Anchor::Center);
  uncollapseSprBG->setScale(.2f);

  m_collapseToggle =
      CCMenuItemToggler::create(collapseSprBG, uncollapseSprBG, this,
                                menu_selector(TitleCell::oniowgoiweogi));
  m_collapseToggle->m_notClickable = true;
  m_collapseToggle->toggle(true);

  m_buttonMenu->addChildAtPosition(m_collapseToggle, geode::Anchor::Center);

  m_nameLabel = cocos2d::CCLabelBMFont::create(title.c_str(), "goldFont.fnt");
  addChild(m_nameLabel);
  addChild(m_buttonMenu);
  m_buttonMenu->setContentSize(m_collapseToggle->getContentSize());
  m_buttonMenu->setAnchorPoint({1,0.5});
  m_buttonMenu->updateLayout();
  m_nameLabel->setPositionX(16);
  m_nameLabel->setPositionY(10);
  m_nameLabel->setAnchorPoint({0, 0.5});
  m_nameLabel->setScale(0.5);

  return true;
}
void TitleCell::oniowgoiweogi(cocos2d::CCObject *) {
  m_collapseToggle->toggle(!m_collapseToggle->isToggled());
  static_cast<CollapsibleContentLayer*>(m_pParent)->onCollapse(nullptr);
}

void MethodCell::onNavigateClick() {
  cocos2d::CCDirector::get()->getRunningScene()->getChildByType<PlaygroundPopup>(0)->navigateToMethodInfo(methodInfo);
};
