#include "WS.hpp"
#include "popups.hpp"
#include "mdtextarea2.hpp"
#include "options_cells.hpp"

OptionCell* createOptionCell(Parameter& p, float width) {
  OptionCell* c = nullptr;
  if (p.type == "boolean") c = BoolOptionCell::create(p);
  if (p.type == "integer") c = IntOptionCell::create(p);
  if (p.type == "number") c = FloatOptionCell::create(p);
  if (p.type == "string") c = StringOptionCell::create(p);

  if (c == nullptr) c = AbsolutelyNothing::create(p);
  c->setContentSize({width, 35});
  return c;
};

void PlaygroundPopup::onExecute(cocos2d::CCObject*) {
  auto protocol = Protocol::get();
  auto funcEntry = protocol->functions.find(currentDomain.domain+"."+currentMethod.name);
  if (funcEntry == protocol->functions.end()) {
    FLAlertLayer::create(
      "Not implemented",
      "The function "+funcEntry->first+" was documented but not implemented in the mod. Sorry!",
      "v"
    )->show();
    return;
  }
  auto func = funcEntry->second.first;

  auto paramsPage = static_cast<CollapsibleContentLayer*>(
    m_infoList->m_contentLayer->getChildByID("params")
  );
  auto p = matjson::Value::object();
  if (paramsPage != nullptr) {
    for (auto* c : paramsPage->m_content) {
      auto e = static_cast<OptionCell*>(c)->createOption();
      p.set(e.first, e.second);
    }
  }
  if (std::holds_alternative<Protocol::ProtocolSyncFunction>(func)) {
    auto j = std::get<Protocol::ProtocolSyncFunction>(func)(p);
    onExecuteFinish(j.isOk() ? j.unwrap() : j.unwrapErr().second);
  } else {
    std::get<Protocol::ProtocolAsyncFunction>(func)(p).addListener([this,p](auto* v){
      onExecuteFinish(v->unwrapOr("errored lol"));
    });
  }
};
void PlaygroundPopup::onExecuteFinish(const matjson::Value& output) {
  auto outputArea = static_cast<geode::SimpleTextArea*>(
    m_infoList->m_contentLayer->getChildByID("output")
  );
  // just in case
  if (outputArea == nullptr) {
    outputArea = geode::SimpleTextArea::create(output.dump(),"consola.fnt"_spr);
    outputArea->setID("output");
    m_infoList->m_contentLayer->addChild(outputArea);
    m_infoList->m_contentLayer->updateLayout();
  } else {
    outputArea->setText(output.dump());
    m_infoList->m_contentLayer->updateLayout();
  }
};

void PlaygroundPopup::setupMethodInfoList(Method& methodInfo) {
  if (currentMethod == methodInfo) {
    geode::log::debug("Skipping construction");
    return;
  }
  m_infoList->m_contentLayer->removeAllChildren();
  currentMethod = methodInfo;

  // title
  auto domainText = cocos2d::CCLabelBMFont::create((currentDomain.domain+".").c_str(), "bigFont.fnt");
  domainText->ignoreAnchorPointForPosition(false);
  domainText->setAnchorPoint({0,0});
  domainText->setColor({ 121, 121, 121 });
  domainText->setScale(0.5);
  auto methodText = cocos2d::CCLabelBMFont::create(methodInfo.name.c_str(), "bigFont.fnt");
  methodText->ignoreAnchorPointForPosition(false);
  methodText->setAnchorPoint({0,0});
  methodText->setPositionX(domainText->getScaledContentWidth());
  methodText->setScale(0.8);
  auto execMenu = CCMenu::create();
  auto btn = CCMenuItemSpriteExtra::create(
    ButtonSprite::create("Execute"), this, menu_selector(PlaygroundPopup::onExecute)
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
  greenStuff->addChild(methodText);
  greenStuff->addChild(execMenu);
  m_infoList->m_contentLayer->addChild(greenStuff);

  // description
  auto descArea = MDDocsTextArea::create(methodInfo.description, {paddedLayerSize.width, 77});
  m_infoList->m_contentLayer->addChild(descArea);

  do {
    bool fl = false;
    if (methodInfo.parameters.size() == 0) break;
    CollapsibleContentLayer* parametersPage = CollapsibleContentLayer::create("Parameters", paddedLayerSize.width);
    parametersPage->setID("params");
    for (auto& p : methodInfo.parameters) {
      auto c = createOptionCell(p,paddedLayerSize.width);
      c->setBGOpacity(fl ? 20 : 60);
      parametersPage->addCell(c);
    }
    parametersPage->updateLayout();
    m_infoList->m_contentLayer->addChild(parametersPage);
  } while (0);
  m_infoList->m_contentLayer->updateLayout();
  m_infoList->moveToTop();
};
