#pragma once
#include "cells.hpp"
#include <Geode/Geode.hpp>

class CollapsibleContentLayer : public geode::GenericContentLayer {
  geode::cocos::CCArrayExt<stupidcell> m_content;
  bool init(std::string title, float width);
  void onCollapse(cocos2d::CCObject*);
  TitleCell* m_titleCell;
  friend class TitleCell;
public:
  void addCell(stupidcell* cell);
  void removeCell(stupidcell* cell);
  void setPosition(cocos2d::CCPoint const& pos) override;

  static CollapsibleContentLayer* create(std::string title, float width) {
    auto ret = new CollapsibleContentLayer();
    if (ret && ret->init(title, width)) {
      ret->autorelease();
      return ret;
    }

    delete ret;
    return nullptr;
  }
};


class DomainContentPopup : public geode::Popup<Domain&> {
  Domain m_domainInfo;

  cocos2d::CCSize paddedLayerSize;
  geode::ScrollLayer *createScrollLayer();

protected:
  bool setup(Domain& info) override;

public:
  static DomainContentPopup* create(Domain& info) {
    auto ret = new DomainContentPopup();
    auto ws = cocos2d::CCDirector::get()->getWinSize();
    if (ret->initAnchored(ws.width/5*4, ws.height/7*6, info)) {
      ret->autorelease();
      return ret;
    }

    delete ret;
    return nullptr;
  }
};


class PlaygroundPopup : public geode::Popup<> {
  geode::ScrollLayer* m_domainList;
  geode::ScrollLayer* m_jList;

  cocos2d::CCSize paddedLayerSize;
  geode::ScrollLayer *createScrollLayer();

  void setupDomainContentList(Domain& domainInfo);
  
  CCMenuItemSpriteExtra* m_backToDomainButton;
  cocos2d::CCMenu* m_backMenu;
  
  void navigateToDomainsList(cocos2d::CCObject *);

protected:
  bool setup() override;

public:
  CloseEventFilter listenForCloseP() {
    return listenForClose();
  }
  static PlaygroundPopup* create() {
    auto ret = new PlaygroundPopup();
    auto ws = cocos2d::CCDirector::get()->getWinSize();
    if (ret->initAnchored(ws.width/5*4, ws.height/7*6)) {
      ret->autorelease();
      return ret;
    }

    delete ret;
    return nullptr;
  }
  void navigateToDomainContents(Domain& info);
};
