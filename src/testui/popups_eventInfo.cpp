#include "popups.hpp"
#include "../WS.hpp"
#include "mdtextarea2.hpp"
#include <map>

std::map<std::string, std::string> queuedEventsContent;

void PlaygroundPopup::setupEventInfoList(Event& eventInfo) {
  dontBlockEventReceivedConstruct = false;
  m_infoList->m_contentLayer->removeAllChildren();
  currentEvent = eventInfo;

  auto ok = currentDomain.domain+"."+currentEvent.name;
  auto c = queuedEventsContent.find(ok);
  bool queued = c != queuedEventsContent.end();

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
  auto btn = CCMenuItemToggler::create(
    ButtonSprite::create("Listen"), ButtonSprite::create("Stop", "goldFont.fnt", "GJ_button_02.png"), 
    this, menu_selector(PlaygroundPopup::onListenToEvent)
  );
  if (queued) {
    btn->toggle(true);
  }
  btn->m_notClickable = true;
  execMenu->addChild(btn);
  execMenu->setID("listen-button-menu");
  btn->setPosition(btn->getContentSize()/2);
  execMenu->setPosition({paddedLayerSize.width, 0});
  execMenu->setContentSize((queued ? btn->m_onButton : btn->m_offButton)->getContentSize());
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

  if (queued) {
    dontBlockEventReceivedConstruct = true;
    onEventReceived(c->second);
  }

  dontBlockEventReceivedConstruct = false;  // ew
  m_infoList->m_contentLayer->updateLayout();

  dontBlockEventReceivedConstruct = true;
};
void PlaygroundPopup::onEventReceived(const std::string& output) {
  if (!dontBlockEventReceivedConstruct) return;
  auto outputArea = static_cast<geode::SimpleTextArea*>(
    m_infoList->m_contentLayer->getChildByID("output")
  );
  // just in case
  if (outputArea == nullptr) {
    dontBlockEventReceivedConstruct = false;
    outputArea = geode::SimpleTextArea::create(output,"consola.fnt"_spr);
    outputArea->setID("output");
    m_infoList->m_contentLayer->addChild(outputArea);
    m_infoList->m_contentLayer->updateLayout();
    dontBlockEventReceivedConstruct = true;
  } else {
    outputArea->setText(output);
    m_infoList->m_contentLayer->updateLayout();
  }
};

void PlaygroundPopup::onListenToEvent(cocos2d::CCObject* b) {
  auto j = currentDomain.domain+"."+currentEvent.name;
  auto btn = static_cast<CCMenuItemToggler*>(b);
  btn->toggle(!btn->isToggled());
  bool queued = btn->isToggled();
  auto execMenu = m_infoList->m_contentLayer->getChildByIDRecursive("listen-button-menu");
  execMenu->setContentSize((queued ? btn->m_onButton : btn->m_offButton)->getContentSize());
  if (queued) {
    auto p = queuedEventsContent.find(j);
    // this will always be true unless some mf decides to flip the toggled state without the callback
    if (p!=queuedEventsContent.end()) queuedEventsContent.erase(p);
  } else {
    // just make it exist
    queuedEventsContent[j] = "";
  }
};


$execute {
  geode::log::warn("steve");
  new geode::EventListener<geode::EventFilter<ProtocolEvent>>(+[](ProtocolEvent* e){
    auto* popup = cocos2d::CCDirector::get()->getRunningScene()->getChildByType<PlaygroundPopup>(0);
    auto eventName = e->eventData["method"].asString().unwrap();
    auto entry = queuedEventsContent.find(eventName);
    if (entry != queuedEventsContent.end()) {
      queuedEventsContent[eventName] = e->eventData.dump();
      if (popup != nullptr) {
        auto ok = popup->currentDomain.domain+"."+popup->currentEvent.name;
        if (ok == eventName) {
          popup->onEventReceived(queuedEventsContent[ok]);
        }
      }
    }

    return geode::ListenerResult::Stop;
  });
}