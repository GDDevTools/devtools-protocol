#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "protocols_json_struct.hpp"

class BezierEasing : public cocos2d::CCActionInterval {
  cocos2d::CCPoint one; cocos2d::CCPoint two;
  cocos2d::CCPoint const& getBezierPoint(float t) {
    cocos2d::CCPoint tmp[4] {{0,0},one,two,{1,1}};
    int i = 3;
    while (i > 0) {
        for (int k = 0; k < i; k++)
            tmp[k] = tmp[k] + (tmp[k+1] - tmp[k]) * t;
        i--;
    }
    return tmp[0];
  }

  cocos2d::CCActionInterval* action;
public:
  bool initWithBezierPoints(cocos2d::CCActionInterval* action, cocos2d::CCPoint o, cocos2d::CCPoint t) {
    if (!cocos2d::CCActionInterval::initWithDuration(action->getDuration())) return false;
    this->action = action;
    one = o;
    two = t;
    return true;
  }
  static BezierEasing* create(cocos2d::CCActionInterval* action, float x1, float y1, float x2, float y2) {
    auto j = new BezierEasing(); // only works on windows
    if (j && j->initWithBezierPoints(action, {x1, y1}, {x2, y2})) {
      j->autorelease();
    } else {
      delete j;
      j = nullptr;
    }
    return j;
  }

  void startWithTarget(cocos2d::CCNode* target) override {
    cocos2d::CCActionInterval::startWithTarget(target);
    action->startWithTarget(target);
  }
  void update(float dd) override {
    action->update(getBezierPoint(dd).y);
  }
};

class PlaygroundPopup : public geode::Popup<> {
  geode::ScrollLayer* m_domainList;
  geode::ScrollLayer* m_amongList;

  cocos2d::CCSize paddedLayerSize;
  geode::ScrollLayer* createScrollLayer() {
    auto layerSize = m_mainLayer->getContentSize();    
    auto brub = geode::ScrollLayer::create(paddedLayerSize);
    brub->setTouchEnabled(true);

    brub->m_contentLayer->setLayout(
        geode::ColumnLayout::create()
            ->setAxisReverse(true)
            ->setAutoGrowAxis(brub->getContentHeight())
            ->setCrossAxisOverflow(false)
            ->setAxisAlignment(geode::AxisAlignment::End)
            ->setGap(0)
    );
    brub->moveToTop();

    return brub;
  }
protected:
  bool setup() override;
  
public:
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
  
  void navigate2(CCObject* obj) {
    m_domainList->runAction(BezierEasing::create(
      cocos2d::CCMoveTo::create(0.3, {-paddedLayerSize.width,0}),
      0.05,0.7,0.1,1.0
    ));
    m_amongList->runAction(BezierEasing::create(
      cocos2d::CCMoveTo::create(0.3, {0,0}),
      0.05,0.7,0.1,1.0
    ));
  }
};

class DomainCell : public cocos2d::CCNode {
  cocos2d::CCMenu* button;
  void mf(cocos2d::CCObject*) {
    cocos2d::CCDirector::get()->getRunningScene()->getChildByType<PlaygroundPopup>(0)->navigate2(nullptr);
  }
  bool init(std::string const& name) {
    auto t = cocos2d::CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
    m_ab = cocos2d::CCLayerColor::create({0,0,0,0});
    addChild(m_ab);
    addChild(t);
    t->setPositionX(16);
    t->setPositionY(15);
    t->setAnchorPoint({0,0.5});
    t->setScale(0.5);

    auto bubu = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    bubu->setFlipX(true);
    auto bub = CCMenuItemSpriteExtra::create(bubu, this, menu_selector(DomainCell::mf));
    button = cocos2d::CCMenu::create();
    button->addChild(bub);
    addChild(button);
    button->setPositionX(16);
    button->setPositionY(15);
    button->setContentSize(bub->getContentSize());
    button->setAnchorPoint({1,0.5});
    button->setScale(0.5);

    setContentSize({150,30}); // height must be 30 or else
    return true;
  }

  cocos2d::CCLayerColor* m_ab;
public:
  void setBGOpacity(GLubyte idk) {
    m_ab->setOpacity(idk);
  }
  void setContentSize(cocos2d::CCSize const& size) override {
    cocos2d::CCNode::setContentSize(size);
    m_ab->setContentSize(size);
    button->setPositionX(size.width - 16*2);
  }

  static DomainCell* create(std::string const& name) {
    auto j = new DomainCell();
    if (j && j->init(name)) {
      j->autorelease();
    } else {
      delete j;
      j = nullptr;
    }
    return j;
  }
};

bool PlaygroundPopup::setup() {
  this->setTitle("Playground");
  auto layerSize = m_mainLayer->getContentSize();    
  paddedLayerSize = layerSize - cocos2d::CCPoint{48,60};
  
  auto astronaut = geode::GenericContentLayer::create(paddedLayerSize.width, paddedLayerSize.height);
  astronaut->setAnchorPoint({0.5,0.5});
  astronaut->setPosition(layerSize/2);
  astronaut->ignoreAnchorPointForPosition(false);

  astronaut->addChild(m_domainList = createScrollLayer());
  astronaut->addChild(m_amongList = createScrollLayer());
  m_mainLayer->addChild(astronaut);
  m_amongList->setPositionX(layerSize.width);

  auto protocols = geode::utils::file::readFromJson<ProtocolsMetadata>(geode::Mod::get()->getResourcesDir() / "protocols.json").unwrap();
  bool flipper_zero = false;
  for (auto& domain : protocols.domains) {
    auto c = DomainCell::create(domain.domain);
    c->setContentSize({layerSize.width, 30});
    m_domainList->m_contentLayer->addChild(c);

    c->setBGOpacity(flipper_zero ? 60 : 20);

    flipper_zero = !flipper_zero;
  }

  m_domainList->m_contentLayer->updateLayout();
  return true;
}


#include <Geode/modify/MenuLayer.hpp>
struct amongus : public geode::Modify<amongus, MenuLayer> {
  bool init() {
    if (!MenuLayer::init()) {
      return false;
    }

    auto myButton = CCMenuItemSpriteExtra::create(
      cocos2d::CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
      this,
      menu_selector(amongus::onPlaygroundButton)
    );

    auto menu = this->getChildByID("bottom-menu");
    menu->addChild(myButton);
    menu->updateLayout();

    return true;
  }

  void onPlaygroundButton(CCObject*) {
    PlaygroundPopup::create()->show();
  }
};
