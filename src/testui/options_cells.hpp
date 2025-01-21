#pragma once

#include "protocols_json_struct.hpp"
#include <matjson.hpp>
#include <string>
#include "cells.hpp"

#define $create_class(Class, ...) \
auto j = new Class; \
if (j->init(__VA_ARGS__)) { \
  j->autorelease(); \
} else { \
  delete j; j = nullptr; \
} \
return j;

class OptionCell : public stupidcell {
public:
  virtual std::pair<std::string, matjson::Value> createOption() {
    return std::make_pair(m_title->getString(), createValue());
  };
  virtual matjson::Value createValue() = 0; // used by array
};

class AbsolutelyNothing : public OptionCell {
public:
  matjson::Value createValue() override {return 0;}
  bool init(std::string t, std::string d) override {
    m_trailingNode = cocos2d::CCNode::create();
    return stupidcell::init(t, d);
  }
  static AbsolutelyNothing* create(Parameter& param) {
    $create_class(AbsolutelyNothing, param.name, param.description);
  };
};

class BoolOptionCell : public OptionCell {
  CCMenuItemToggler* m_button;
protected:
  bool init(std::string title, std::string description = "") override;

public:
  static BoolOptionCell* create(Parameter& param) {
    $create_class(BoolOptionCell, param.name, param.description);
  };
  matjson::Value createValue() override {
    return m_button->isToggled();
  };
};

class IntOptionCell : public OptionCell {
  geode::TextInput* m_input;
protected:
  bool init(std::string title, std::string description = "") override;

public:
  static IntOptionCell* create(Parameter& param) {
    $create_class(IntOptionCell, param.name, param.description);
  };
  matjson::Value createValue() override {
    return std::stoi(m_input->getString());
  };
};
class FloatOptionCell : public OptionCell {
  geode::TextInput* m_input;
protected:
  bool init(std::string title, std::string description = "") override;

public:
  static FloatOptionCell* create(Parameter& param) {
    $create_class(FloatOptionCell, param.name, param.description);
  };
  matjson::Value createValue() override {
    return std::stod(m_input->getString());
  };
};

class StringOptionCell : public OptionCell {
  geode::TextInput* m_input;
protected:
  bool init(std::string title, std::string description = "") override;

public:
  static StringOptionCell* create(Parameter& param) {
    $create_class(StringOptionCell, param.name, param.description);
  };
  matjson::Value createValue() override {
    return m_input->getString();
  };
};