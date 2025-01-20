#include "popups.hpp"
#include "../WS.hpp"
#include "mdtextarea2.hpp"
#include <map>

std::map<std::string, std::string> queuedEventsContent;

void PlaygroundPopup::setupEventInfoList(Event& eventInfo) {
  m_infoList->m_contentLayer->removeAllChildren();
  currentEvent = eventInfo;

  // title
  auto domainText = cocos2d::CCLabelBMFont::create((currentDomain.domain+".").c_str(), "bigFont.fnt");
  domainText->ignoreAnchorPointForPosition(false);
  domainText->setAnchorPoint({0,0});
  domainText->setColor({ 121, 121, 121 });
  domainText->setScale(0.5);
  auto eventText = cocos2d::CCLabelBMFont::create(eventInfo.name.c_str(), "bigFont.fnt");
  eventText->ignoreAnchorPointForPosition(false);
  eventText->setAnchorPoint({0,0});
  eventText->setPositionX(domainText->getScaledContentWidth());
  eventText->setScale(0.8);
  auto execMenu = CCMenu::create();
  auto btn = CCMenuItemSpriteExtra::create(
    ButtonSprite::create("Listen"), this, nullptr
  );
  execMenu->addChild(btn);
  btn->setPosition(btn->getContentSize()/2);
  execMenu->setPosition({paddedLayerSize.width, 0});
  execMenu->setContentSize(btn->getContentSize());
  execMenu->setAnchorPoint({1,0});
  execMenu->ignoreAnchorPointForPosition(false);
  execMenu->setScale(0.7);

  auto greenStuff = CCNode::create();
  greenStuff->setContentSize(domainText->getContentSize());
  greenStuff->addChild(domainText);
  greenStuff->addChild(eventText);
  greenStuff->addChild(execMenu);
  m_infoList->m_contentLayer->addChild(greenStuff);

  // description
  auto descArea = MDDocsTextArea::create(eventInfo.description, {paddedLayerSize.width, 77});
  m_infoList->m_contentLayer->addChild(descArea);
};
