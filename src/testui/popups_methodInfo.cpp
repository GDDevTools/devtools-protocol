#include "popups.hpp"
#include "mdtextarea2.hpp"
#include "options_cells.hpp"

OptionCell* createOptionCell(Parameter& param, float width) {
  OptionCell* c = nullptr;
  if (param.type == "boolean") c = BoolOptionCell::create(p);
  if (param.type == "integer") c = IntOptionCell::create(p);
  if (param.type == "number") c = FloatOptionCell::create(p);
  c->setContentSize({width, 35});
  return c;
};

void PlaygroundPopup::onExecute(cocos2d::CCObject*) {
  auto paramsPage = static_cast<CollapsibleContentLayer*>(
    m_idkList->m_contentLayer->getChildByID("params");
  );
};

void PlaygroundPopup::setupMethodInfoList(Method& methodInfo) {
  if (currentMethod == methodInfo) {
    geode::log::debug("Skipping construction");
    return;
  }
  m_idkList->m_contentLayer->removeAllChildren();
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
    ButtonSprite::create("Execute"), this, nullptr
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
  m_idkList->m_contentLayer->addChild(greenStuff);

  // description
  auto descArea = MDDocsTextArea::create(methodInfo.description, {paddedLayerSize.width, 77});
  m_idkList->m_contentLayer->addChild(descArea);

  do {
    bool fl = false;
    if (methodInfo.parameters.size() == 0) break;
    auto parametersPage = CollapsibleContentLayer::create("Parameters", paddedLayerSize.width);
    parametersPage->setID("params");
    for (auto& p : methodInfo.parameters) {
      auto c = createOptionCell(p,paddedLayerSize.width);
      c->setBGOpacity(fl ? 20 : 60);
      parametersPage->addCell(c);
    }
    parametersPage->updateLayout();
    m_idkList->m_contentLayer->addChild(parametersPage);
  } while (0);
  static_cast<geode::ColumnLayout*>(m_idkList->m_contentLayer->getLayout())
  ->setCrossAxisAlignment(geode::AxisAlignment::Start)
  ->setCrossAxisLineAlignment(geode::AxisAlignment::Start)
  ->setGap(8);
  m_idkList->m_contentLayer->updateLayout();
  m_idkList->moveToTop();
};