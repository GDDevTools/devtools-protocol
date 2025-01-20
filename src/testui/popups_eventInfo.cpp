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

  auto ok = currentDomain.domain+"."+currentEvent.name;
  auto c = queuedEventsContent.find(ok);
  if (c != queuedEventsContent.end()) {
    onEventReceived(c->second);
  }

  m_infoList->m_contentLayer->updateLayout();
};
void PlaygroundPopup::onEventReceived(const std::string& output) {
  auto outputArea = static_cast<geode::SimpleTextArea*>(
    m_infoList->m_contentLayer->getChildByID("output")
  );
  // just in case
  if (outputArea == nullptr) {
    outputArea = geode::SimpleTextArea::create(output,"consola.fnt"_spr);
    outputArea->setID("output");
    m_infoList->m_contentLayer->addChild(outputArea);
    m_infoList->m_contentLayer->updateLayout();
  } else {
    outputArea->setText(output);
    m_infoList->m_contentLayer->updateLayout();
  }
};

$execute {
  new geode::EventListener<geode::EventFilter<ProtocolEvent>>(+[](ProtocolEvent* e){
    auto* popup = cocos2d::CCDirector::get()->getRunningScene()->getChildByType<PlaygroundPopup>(0);
    auto eventName = e->eventData["name"].asString().unwrap();
    auto ok = popup->currentDomain.domain+"."+popup->currentEvent.name;
    if (popup && ok == eventName) {
      popup->onEventReceived(e->eventData.dump());
    }
    else {
      queuedEventsContent[ok] = e->eventData.dump();
    }

    return geode::ListenerResult::Stop;
  });
}