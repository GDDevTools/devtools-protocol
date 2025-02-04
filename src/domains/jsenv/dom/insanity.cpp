#include "../../external/rapidxml/rapidxml.hpp"
#include <Geode/Result.hpp>
#include <Geode/Geode.hpp>
#include "insanity.hpp"

std::string value_of(rapidxml::xml_base<>* g) {
    if (g)
        return std::string(g->value(), g->value_size());
    return "";
}
std::string name_of(rapidxml::xml_base<>* g) {
    if (g)
        return std::string(g->name(), g->name_size());
    return "";
}


using namespace cocos2d;
cocos2d::CCNode* whatNodeCouldThisBe(rapidxml::xml_node<>* node) {
    auto name = name_of(node);
    if (name == "scene") return CCScene::create();
    if (name == "p") return CCLabelBMFont::create(value_of(node).c_str(),"bigFont.fnt");
    if (name == "img") {
      auto img = value_of(node->first_attribute("src"));
      CCNode* spr = nullptr;
      if (node->first_attribute("sizable") == nullptr) {
        spr = CCSprite::create(img.c_str());
        if (spr == nullptr) {
          spr = CCSprite::createWithSpriteFrameName(img.c_str());
        }
      } else {
        spr = extension::CCScale9Sprite::create(img.c_str());
        if (spr == nullptr) {
          spr = extension::CCScale9Sprite::createWithSpriteFrameName(img.c_str());
        }
      }

      if (spr == nullptr) spr = CCNode::create();
      return spr;
    }

    return CCNode::create();
}


#include "domains/external/css/include/cssparser/CSSParser.h"
#include "domains/external/css/include/resolvers/INodeSelector.h"

#define $getMapValue(map, key, defaultTo) ({                                 \
  auto GEODE_CONCAT(res, __LINE__) = map.find(key);                          \
  if (GEODE_CONCAT(res, __LINE__) != map.end())                              \
    GEODE_CONCAT(res, __LINE__)->second;                                     \
                                                                             \
  defaultTo;                                                                 \
})
std::vector<std::string> split(std::string s, std::string delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
      token = s.substr (pos_start, pos_end - pos_start);
      pos_start = pos_end + delim_len;
      res.push_back (token);
  }

  res.push_back (s.substr (pos_start));
  return res;
}
class XHTMLNode : public INodeSelector {};
cocos2d::CCNode* createNode(rapidxml::xml_node<>* node, int idx) {
  std::string style = "c {"+value_of(node->first_attribute("style"))+"}";
  future::CSSParser p;
  p.parseByString(style);
  auto m = p.getSelectors().front()->getRuleDataMap();
  float xpos = std::stof($getMapValue(m, "x", "0"));
  float ypos = std::stof($getMapValue(m, "y", "0"));
  float width = std::stof($getMapValue(m, "width", "-1"));
  float height = std::stof($getMapValue(m, "height", "-1"));
  float scalex = 1;
  float scaley = 1;

  std::vector<std::string> scale = split($getMapValue(m, "scale", "1"), " ");
  if (scale.size() == 1) {
    scalex = scaley = std::stof(scale[0]);
  } else {
    scalex = std::stof(scale[0]);
    scaley = std::stof(scale[1]);
  }

  auto ret = whatNodeCouldThisBe(node);
  ret->setPosition({xpos,ypos});
  ret->setContentSize({
    width == -1 ? ret->getContentWidth() : width,
    height == -1 ? ret->getContentHeight() : height
  });
  ret->setScale(scalex, scaley);


  auto c = node->first_node();
  int cidx = 0;
  while (c) {
    ret->addChild(createNode(c,cidx++));
    c = c->next_sibling();
  }
  
  return ret;
}

cocos2d::CCNode* constructNodeFrom(std::string html) {
    rapidxml::xml_document<> doc;
    doc.parse<rapidxml::parse_no_string_terminators>((char*)html.c_str());

    return createNode(doc.first_node(), 0);
}
