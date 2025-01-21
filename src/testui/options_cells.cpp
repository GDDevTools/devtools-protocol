#include "options_cells.hpp"
#include <cinttypes>

bool BoolOptionCell::init(std::string title, std::string description) {
  m_trailingNode = CCMenu::create();
  m_button = CCMenuItemToggler::create(
      CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
      CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"), this,
      nullptr);
  m_button->setPosition(m_button->getContentSize() / 2);
  m_trailingNode->addChild(m_button);
  m_trailingNode->setContentSize(m_button->getContentSize());

  return stupidcell::init(title, description);
}
bool IntOptionCell::init(std::string title, std::string description) {
  m_trailingNode = m_input = geode::TextInput::create(77, "0");
  m_input->setCallback([this](auto const& str) {
    char* j;
    long long idk = strtoimax(str.c_str(), &j, 7);
    
    if (*j != 0) {
      m_input->setString(str);
    }
  });

  return stupidcell::init(title, description);
}
bool FloatOptionCell::init(std::string title, std::string description) {
  m_trailingNode = m_input = geode::TextInput::create(77, "0");
  m_input->setCallback([this](auto const& str) {
    char* j;
    double idk = strtod(str.c_str(), &j);
    
    if (*j != 0) {
      m_input->setString(str);
    }
  });

  return stupidcell::init(title, description);
}
bool StringOptionCell::init(std::string title, std::string description) {
  m_trailingNode = m_input = geode::TextInput::create(250, "");
  m_input->setCallback([this](std::string const& str){m_input->setString(str);});

  return stupidcell::init(title, description);
}
