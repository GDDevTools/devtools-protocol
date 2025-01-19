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
