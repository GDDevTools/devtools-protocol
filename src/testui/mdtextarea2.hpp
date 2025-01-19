#pragma once

#include <Geode/ui/MDTextArea.hpp>

class MDDocsTextArea : public geode::MDTextArea {
protected:
  void onLink(cocos2d::CCObject *j);
  bool init2(std::string const &str, cocos2d::CCSize const &size);

public:
  static MDDocsTextArea* create(std::string const& str, cocos2d::CCSize const& size) {
    auto ret = new MDDocsTextArea;
    if (ret->init2(str, size)) {
      ret->autorelease();
      return ret;
    }
    delete ret;
    return nullptr;
  }
};