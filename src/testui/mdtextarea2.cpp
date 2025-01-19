#include "mdtextarea2.hpp"
#include "popups.hpp"

void MDDocsTextArea::onLink(cocos2d::CCObject *j) {
  auto href = static_cast<cocos2d::CCString *>(
      static_cast<cocos2d::CCNode *>(j)->getUserObject());
  std::string url(href->getCString());
  if (url.starts_with("method:")) {
  } else {
    auto layer = FLAlertLayer::create(
      this, "Hold Up!",
      "Links are spooky! Are you sure you want to go to <cy>" + url + "</c>?",
      "Cancel", "Yes", 360.f);
    layer->setUserObject(href);
    layer->show();
  }
}
float getAnchoredDistance(cocos2d::CCNode* node) {
  return node->getAnchorPoint().y*node->getScaledContentHeight();
};
bool MDDocsTextArea::init2(std::string const &str,
                           cocos2d::CCSize const &size) {
  if (!init(str, size))
    return false;
  m_content->removeFromParentAndCleanup(false);
  this->addChild(m_content);
  this->getScrollLayer()->setVisible(false);

  this->setAnchorPoint({0.5, 0});

  float maxHeight = 0;
  float originalHeight = m_content->getContentHeight();
  float padding = 
    m_content->getPositionY()+getAnchoredDistance(m_content)
    - 
    m_bgSprite->getPositionY()+getAnchoredDistance(m_bgSprite)
  ;
  geode::log::debug("{}", padding);

  for (auto *c :
       geode::cocos::CCArrayExt<cocos2d::CCNode *>(m_content->getChildren())) {
    float ch = c->getScaledContentHeight();
    float horrorOperation = ch + (originalHeight - c->getPositionY() -
                                  (c->getAnchorPoint().y * ch));
    maxHeight = std::max(maxHeight, horrorOperation);
  }
  float diff = originalHeight - maxHeight;
  for (auto *c :
       geode::cocos::CCArrayExt<cocos2d::CCNode *>(m_content->getChildren())) {
    c->setPositionY(c->getPositionY() - diff);
  }
  m_content->setContentHeight(maxHeight);
  m_bgSprite->setContentHeight(maxHeight / m_bgSprite->getScaleY() + padding);
  this->setContentSize(m_bgSprite->getScaledContentSize());
  m_bgSprite->setPosition(getContentSize()/2);
  m_content->setPosition(getContentSize()/2);
  m_content->ignoreAnchorPointForPosition(false);

  return true;
}
