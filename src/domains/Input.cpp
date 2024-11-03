#include "../WS.hpp"
// clangd is bad
#include "Geode/cocos/robtop/mouse_dispatcher/CCMouseDispatcher.h"
#include "Geode/cocos/touch_dispatcher/CCTouchDispatcher.h"
#include "Geode/utils/cocos.hpp"
#include <Geode/cocos/CCDirector.h>
#include <Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDispatcher.h>

inline bool bit(int n, int k) {
  return (n & ( 1 << k )) >> k;
}

/// a function from cocos2d might exists
inline cocos2d::enumKeyCodes keyEnumForChar(char c) {
  switch (c) {
    case 'a': return cocos2d::KEY_A;
    case 'b': return cocos2d::KEY_B;
    case 'c': return cocos2d::KEY_C;
    case 'd': return cocos2d::KEY_D;
    case 'e': return cocos2d::KEY_E;
    case 'f': return cocos2d::KEY_F;
    case 'g': return cocos2d::KEY_G;
    case 'h': return cocos2d::KEY_H;
    case 'i': return cocos2d::KEY_I;
    case 'j': return cocos2d::KEY_J;
    case 'k': return cocos2d::KEY_K;
    case 'l': return cocos2d::KEY_L;
    case 'm': return cocos2d::KEY_M;
    case 'n': return cocos2d::KEY_N;
    case 'o': return cocos2d::KEY_O;
    case 'p': return cocos2d::KEY_P;
    case 'q': return cocos2d::KEY_Q;
    case 'r': return cocos2d::KEY_R;
    case 's': return cocos2d::KEY_S;
    case 't': return cocos2d::KEY_T;
    case 'u': return cocos2d::KEY_U;
    case 'v': return cocos2d::KEY_V;
    case 'w': return cocos2d::KEY_W;
    case 'x': return cocos2d::KEY_X;
    case 'y': return cocos2d::KEY_Y;
    case 'z': return cocos2d::KEY_Z;

    case '0': return cocos2d::KEY_Zero;
    case '1': return cocos2d::KEY_One;
    case '2': return cocos2d::KEY_Two;
    case '3': return cocos2d::KEY_Three;
    case '4': return cocos2d::KEY_Four;
    case '5': return cocos2d::KEY_Five;
    case '6': return cocos2d::KEY_Six;
    case '7': return cocos2d::KEY_Seven;
    case '8': return cocos2d::KEY_Eight;
    case '9': return cocos2d::KEY_Nine;

    case '-': return cocos2d::KEY_Subtract;
    case '*': return cocos2d::KEY_Multiply;
    case '+': return cocos2d::KEY_Add;
    case '/': return cocos2d::KEY_Divide;
  }
  return cocos2d::KEY_Unknown;
}

$domainMethod(handleKeyDownEvent) {
  auto disp = cocos2d::CCDirector::get()->getKeyboardDispatcher();
  auto modifiersV = params["modifiers"];
  if (modifiersV.is_number()) {
    int mod = modifiersV.as_int();
    disp->updateModifierKeys(
      disp->getShiftKeyPressed() || !bit(mod,8),
      disp->getControlKeyPressed() || !bit(mod,2),
      disp->getAltKeyPressed() || !bit(mod,1),
      disp->getCommandKeyPressed() || !bit(mod,4)
    );
  }
  auto codeV = params["code"];
  if (codeV.is_string()) {
    disp->dispatchKeyboardMSG(
      keyEnumForChar(std::tolower(codeV.as_string()[0])),
      true, false
    );
  }
  return geode::Ok(matjson::Object{});
}
$domainMethod(handleKeyUpEvent) {
  auto disp = cocos2d::CCDirector::get()->getKeyboardDispatcher();
  auto modifiersV = params["modifiers"];
  if (modifiersV.is_number()) {
    int mod = modifiersV.as_int();
    disp->updateModifierKeys(
      disp->getShiftKeyPressed() && !bit(mod,8),
      disp->getControlKeyPressed() && !bit(mod,2),
      disp->getAltKeyPressed() && !bit(mod,1),
      disp->getCommandKeyPressed() && !bit(mod,4)
    );
  }
  auto codeV = params["code"];
  if (codeV.is_string()) {
    disp->dispatchKeyboardMSG(
      keyEnumForChar(std::tolower(codeV.as_string()[0])),
      false, false
    );
  }
  return geode::Ok(matjson::Object{});
}
$domainMethod(handleCharEvent) {
  auto disp = cocos2d::CCDirector::get()->getKeyboardDispatcher();

  auto modifiersV = params["modifiers"];
  bool shift = disp->getShiftKeyPressed();
  bool ctrl = disp->getControlKeyPressed();
  bool alt = disp->getAltKeyPressed();
  bool cmd = disp->getCommandKeyPressed();
  if (modifiersV.is_number()) {
    int mod = modifiersV.as_int();
    disp->updateModifierKeys(
      shift || !bit(mod,8),
      ctrl || !bit(mod,2),
      alt || !bit(mod,1),
      cmd || !bit(mod,4)
    );
  }

  auto codeV = params["code"];
  if (codeV.is_string()) {
    for (char c : codeV.as_string()) {
      disp->dispatchKeyboardMSG(
        keyEnumForChar(std::tolower(c)),
        false, false
      );
    }
  }
  disp->updateModifierKeys(shift,ctrl,alt,cmd);

  return geode::Ok(matjson::Object{});
}
$domainMethod(dispatchKeyEvent) {
  auto typeV = params["type"];
  if (typeV.is_null()) {return errors::requiredParameter("type");}
  auto type = typeV.as_string();
  if (type == "keyDown") return handleKeyDownEvent(params);
  if (type == "keyUp") return handleKeyUpEvent(params);
  if (type == "char") return handleCharEvent(params);

  return errors::invalidParameter("Unknown key event type '"+type+"'");
}
const std::string mouseButton[8] = {
  "left", "middle", "right", "back", "forward", "6", "7", "8"
};
int touchId = 0;
geode::cocos::CCArrayExt<cocos2d::CCTouch*> touches;
$domainMethod(dispatchMouseEvent) {
  auto typeV = params.find("type");
  if (typeV == params.end()) return errors::requiredParameter("type");
  auto type = typeV->second;
  if (type != "mouseWheel") {
    float x,y;
    if (true) {
      if (params.find("x") == params.end()) return errors::requiredParameter("x");
      x = params["x"].as_double();
      if (params.find("y") == params.end()) return errors::requiredParameter("y");
      y = params["y"].as_double();
    }
    if (params.find("button") == params.end()) return errors::requiredParameter("button");
    std::string button = params["button"].as_string();
    int buttonId = std::distance(mouseButton, std::find(mouseButton, mouseButton + 8, button));

    cocos2d::CCTouch* touch;
    if (type == "mousePressed"){
      touch = new cocos2d::CCTouch();
      touches.push_back(touch);
      //touch->release(); // risky
    } else {
      // TODO: make mouse move actually do something (gd!lazer) by calling ccegl shit
      if (touches.size() == 0) return geode::Ok(matjson::Object{});
      if (type == "mouseReleased") touch = touches.pop_back();
      else touch = touches[touches.size()-1];
    }
    touch->setTouchInfo(touchId, x, y);
    auto set = cocos2d::CCSet::create();
    set->addObject(touch);
    if (type == "mousePressed") {
      cocos2d::CCDirector::get()->getTouchDispatcher()->touchesBegan(set,new cocos2d::CCEvent());
    } else if (type == "mouseReleased") {
      cocos2d::CCDirector::get()->getTouchDispatcher()->touchesEnded(set,new cocos2d::CCEvent());
      //cocos2d::CCDirector::get()->getTouchDispatcher()->touchesCancelled(set,new cocos2d::CCEvent());
    } else {
      cocos2d::CCDirector::get()->getTouchDispatcher()->touchesMoved(set,new cocos2d::CCEvent());
    }
  } else {
    int x = 0,y = 0;
    if (true) {
      auto x2 = params["deltaX"];
      if (!x2.is_null()) x = x2.as_double();
      auto y2 = params["deltaY"];
      if (!y2.is_null()) y = y2.as_double();
    }
    cocos2d::CCDirector::get()->getMouseDispatcher()->dispatchScrollMSG(x,y);
  }
  return geode::Ok(matjson::Object{});
}

$execute {
  auto p = Protocol::get();
  p->registerFunction("Input.dispatchKeyEvent", &dispatchKeyEvent);
  p->registerFunction("Input.dispatchMouseEvent", &dispatchMouseEvent);
}
//{"id":0,"method":"Input.dispatchMouseEvent","params":{"type":"mousePressed","x":284,"y":160}}
