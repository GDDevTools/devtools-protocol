#pragma once
#include "cells.hpp"
#include <Geode/Geode.hpp>

class CollapsibleContentLayer : public geode::GenericContentLayer {
  geode::cocos::CCArrayExt<stupidcell> m_content;
  bool init(std::string title, float width);
  void onCollapse(cocos2d::CCObject*);
  TitleCell* m_titleCell;
  friend class TitleCell;
  friend class PlaygroundPopup;
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

class PlaygroundPopup : public geode::Popup<> {
  geode::ScrollLayer* m_domainList;
  geode::ScrollLayer* m_jList;
  geode::ScrollLayer* m_infoList;

  cocos2d::CCSize paddedLayerSize;
  geode::ScrollLayer *createScrollLayer();

  void setupDomainContentList(Domain& domainInfo);
  void setupMethodInfoList(Method& methodInfo);
  void setupEventInfoList(Event& eventInfo);
  
  CCMenuItemSpriteExtra* m_backToDomainButton;
  CCMenuItemSpriteExtra* m_backToDomainContentsButton;
  cocos2d::CCMenu* m_backMenu;
  
  void navigateToDomainsList(cocos2d::CCObject *);
  void navigateBackToDomainContents(cocos2d::CCObject *);

  Domain currentDomain;
  Method currentMethod;
  Event currentEvent;

  void onExecute(cocos2d::CCObject*);
  void onExecuteFinish(const matjson::Value& output);
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
  void navigateToEventInfo(Event& info);
  void navigateToMethodInfo(Method& info);
};
