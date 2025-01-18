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
  if (m_pParent!=nullptr) m_pParent->updateLayout();
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

  m_domainList->setAnchorPoint({0.5, 0.5});
  m_domainList->setPosition(layerSize / 2);
  m_domainList->ignoreAnchorPointForPosition(false);

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

  m_jList->setAnchorPoint({0.5, 0.5});
  m_jList->setPosition(layerSize / 2);
  m_jList->ignoreAnchorPointForPosition(false);

  m_mainLayer->addChild(m_jList);
  m_jList->setVisible(false);


  m_backToDomainButton = CCMenuItemSpriteExtra::create(
    CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"), this,
    menu_selector(PlaygroundPopup::navigateToDomainsList)
  );

  auto j = m_backToDomainButton->getContentSize();
  m_backMenu = cocos2d::CCMenu::create();
  m_backMenu->setContentSize(j);
  m_backMenu->addChild(m_backToDomainButton);
  m_backToDomainButton->setPosition(j/2);
  m_backToDomainButton->setVisible(false);

  return true;
}
void PlaygroundPopup::setupDomainContentList(Domain& domainInfo) {
  if (m_jList->getUserData() != nullptr && *static_cast<Domain*>(m_jList->getUserData()) == domainInfo) {
    geode::log::debug("Skipping construction");
    return;
  }
  m_jList->m_contentLayer->removeAllChildren();
  m_jList->setUserData(&domainInfo);
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
  methodsList->setPosition(methodsList->getPosition());

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

  return brub;
}
void PlaygroundPopup::navigateToDomainContents(Domain& info) {
  setupDomainContentList(info);
  m_domainList->setVisible(false);
  m_jList->setVisible(true);
  setTitle("Playground: "+info.domain+" method");
};
void PlaygroundPopup::navigateToDomainsList(cocos2d::CCObject *) {
  m_domainList->setVisible(true);
  m_jList->setVisible(false);
  setTitle("Playground");
};
