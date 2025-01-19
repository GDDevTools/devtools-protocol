#include "popups.hpp"
#include "Geode/utils/cocos.hpp"

bool CollapsibleContentLayer::init(std::string title, float width) {
  if (!initWithColor({0,0,0,0}, width, 20)) return false;

  m_titleCell = TitleCell::create(title);
  m_titleCell->setContentSize({width,20});
  addChild(m_titleCell);
  setLayout(
    geode::ColumnLayout::create()
        ->setAxisReverse(true)
        ->setAutoGrowAxis(20)
        ->setCrossAxisOverflow(false)
        ->setAxisAlignment(geode::AxisAlignment::End)
        ->setGap(0)
  );
  
  setAnchorPoint({0,0});

  return true;
};
void CollapsibleContentLayer::setPosition(cocos2d::CCPoint const& position) {
  CCLayerColor::setPosition(position);
}
void CollapsibleContentLayer::onCollapse(cocos2d::CCObject*) {
  bool tog = !m_titleCell->isCollapsed();
  for (auto* c : m_content) {
    if (tog) {
      addChild(c);
    } else {
      removeChild(c);
    }
  }
  updateLayout();
  if (m_pParent!=nullptr) {
    m_pParent->updateLayout();
    if (auto s = geode::cast::typeinfo_cast<geode::ScrollLayer*>(m_pParent->getParent())) {
      s->moveToTop();
    }
  }
};
void CollapsibleContentLayer::addCell(stupidcell* cell) {
  if (!m_titleCell->isCollapsed()) addChild(cell);
  cell->setVisible(true);
  m_content.push_back(cell);
};
void CollapsibleContentLayer::removeCell(stupidcell* cell) {
  if (!m_titleCell->isCollapsed()) removeChild(cell);
  m_content.inner()->removeObject(cell);
};

bool PlaygroundPopup::setup() {
  this->setTitle("Playground");
  auto layerSize = m_mainLayer->getContentSize();
  paddedLayerSize = layerSize - cocos2d::CCPoint{48, 60};

  m_domainList = createScrollLayer();

  m_mainLayer->addChild(m_domainList);

  auto protocols = geode::utils::file::readFromJson<ProtocolsMetadata>(
                      geode::Mod::get()->getResourcesDir() / "protocols.json"
                          ).unwrap();
  bool flipper_zero = false;
  for (auto &domain : protocols.domains) {
    auto c = DomainCell::create(domain);
    c->setContentSize({layerSize.width, 30});
    m_domainList->m_contentLayer->addChild(c);

    c->setBGOpacity(flipper_zero ? 60 : 20);

    flipper_zero = !flipper_zero;
  }

  m_domainList->m_contentLayer->updateLayout();

  m_jList = createScrollLayer();
  m_jList->setVisible(false);
  m_mainLayer->addChild(m_jList);
  
  m_idkList = createScrollLayer();
  m_idkList->setVisible(false);
  m_mainLayer->addChild(m_idkList);

  m_backToDomainButton = CCMenuItemSpriteExtra::create(
    CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"), this,
    menu_selector(PlaygroundPopup::navigateToDomainsList)
  );
  m_backToDomainContentsButton = CCMenuItemSpriteExtra::create(
    CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"), this,
    menu_selector(PlaygroundPopup::navigateBackToDomainContents)
  );

  auto j = m_backToDomainButton->getContentSize();

  m_backMenu = cocos2d::CCMenu::create();
  m_backMenu->setContentSize(j);
  
  m_backMenu->addChild(m_backToDomainButton);
  m_backToDomainButton->setPosition(j/2);
  m_backToDomainButton->setVisible(false);
  m_backMenu->addChild(m_backToDomainContentsButton);
  m_backToDomainContentsButton->setPosition(j/2);
  m_backToDomainContentsButton->setVisible(false);

  m_backMenu->setPosition({(layerSize.width-paddedLayerSize.width)/2,m_title->getPositionY()});
  m_backMenu->setScale(0.57);
  m_backMenu->ignoreAnchorPointForPosition(false);
  m_backMenu->setAnchorPoint({0,0.5});
  m_mainLayer->addChild(m_backMenu);
  return true;
}

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

  auto greenStuff = CCNode::create();
  greenStuff->setContentSize(domainText->getContentSize());
  greenStuff->addChild(domainText);
  greenStuff->addChild(methodText);
  m_idkList->m_contentLayer->addChild(greenStuff);

  // description
  auto descArea = geode::MDTextArea::create(methodInfo.description, {paddedLayerSize.width, 77});
  m_idkList->m_contentLayer->addChild(descArea);

  static_cast<geode::ColumnLayout*>(m_idkList->m_contentLayer->getLayout())
  ->setCrossAxisAlignment(geode::AxisAlignment::Start)
  ->setCrossAxisLineAlignment(geode::AxisAlignment::Start)
  ->setGap(8);
  m_idkList->m_contentLayer->updateLayout();
};

void PlaygroundPopup::setupDomainContentList(Domain& domainInfo) {
  if (currentDomain == domainInfo) {
    geode::log::debug("Skipping construction");
    return;
  }
  m_jList->m_contentLayer->removeAllChildren();
  
  // description
  auto descArea = geode::MDTextArea::create(domainInfo.description, {paddedLayerSize.width, 77});
  m_jList->m_contentLayer->addChild(descArea);
  descArea->getScrollLayer()->setMouseEnabled(false);

  currentDomain = domainInfo;
  bool flipper_zero = false;

  auto methodsList = CollapsibleContentLayer::create("Methods", paddedLayerSize.width);

  for (auto &method : domainInfo.methods) {
    auto c = MethodCell::create(method);
    c->setContentSize({paddedLayerSize.width, 30});
    methodsList->addCell(c);

    c->setBGOpacity(flipper_zero ? 60 : 20);

    flipper_zero = !flipper_zero;
  }

  m_jList->m_contentLayer->addChild(methodsList);
  methodsList->updateLayout();

  m_jList->m_contentLayer->updateLayout();
}
geode::ScrollLayer *PlaygroundPopup::createScrollLayer() {
  auto brub = geode::ScrollLayer::create(paddedLayerSize);
  brub->setTouchEnabled(true);

  brub->m_contentLayer->setLayout(
      geode::ColumnLayout::create()
          ->setAxisReverse(true)
          ->setAutoGrowAxis(brub->getContentHeight())
          ->setCrossAxisOverflow(false)
          ->setAxisAlignment(geode::AxisAlignment::End)
          ->setGap(0));
  brub->moveToTop();
  brub->setAnchorPoint({0.5, 0.5});
  brub->setPosition(m_mainLayer->getContentSize() / 2);
  brub->ignoreAnchorPointForPosition(false);

  return brub;
}
void PlaygroundPopup::navigateToDomainContents(Domain& info) {
  setupDomainContentList(info);
  m_domainList->setVisible(false);
  m_jList->setVisible(true);
  m_backToDomainButton->setVisible(true);
  setTitle("Playground: "+info.domain+" domain");
};
void PlaygroundPopup::navigateBackToDomainContents(cocos2d::CCObject *) {
  m_idkList->setVisible(false);
  m_jList->setVisible(true);
  m_backToDomainContentsButton->setVisible(false);
  m_backToDomainButton->setVisible(true);
  setTitle("Playground: "+currentDomain.domain+" domain");
};
void PlaygroundPopup::navigateToDomainsList(cocos2d::CCObject *) {
  m_domainList->setVisible(true);
  m_jList->setVisible(false);
  m_backToDomainButton->setVisible(false);
  setTitle("Playground");
};
void PlaygroundPopup::navigateToMethodInfo(Method& method) {
  setupMethodInfoList(method);
  m_idkList->setVisible(true);
  m_jList->setVisible(false);
  m_backToDomainButton->setVisible(false);
  m_backToDomainContentsButton->setVisible(true);
  setTitle(currentDomain.domain+"."+method.name+" method");
};