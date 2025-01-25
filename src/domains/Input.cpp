#include "../WS.hpp"
// clangd is bad
#include "Geode/cocos/robtop/mouse_dispatcher/CCMouseDispatcher.h"
#include "Geode/cocos/touch_dispatcher/CCTouchDispatcher.h"
#include <Geode/utils/cocos.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/cocos/CCDirector.h>
#include <Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDispatcher.h>
#include <chrono>

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

// code dupe i know
$domainMethod(handleKeyDownEvent) {
  auto disp = cocos2d::CCDirector::get()->getKeyboardDispatcher();
  auto modifiersV = params["modifiers"];
  if (modifiersV.isNumber()) {
    int mod = modifiersV.asInt().unwrap();
    disp->updateModifierKeys(
      disp->getShiftKeyPressed() || !bit(mod,8),
      disp->getControlKeyPressed() || !bit(mod,2),
      disp->getAltKeyPressed() || !bit(mod,1),
      disp->getCommandKeyPressed() || !bit(mod,4)
    );
  }
  auto code = params["code"].asString();
  if (code.isOk()) {
    auto j = code.unwrap();
    geode::queueInMainThread([j,disp]{
      disp->dispatchKeyboardMSG(
        keyEnumForChar(std::tolower(j[0])),
        true, false
      );
    });
  }
  return emptyResponse();
}
$domainMethod(handleKeyUpEvent) {
  auto disp = cocos2d::CCDirector::get()->getKeyboardDispatcher();
  auto modifiersV = params["modifiers"];
  if (modifiersV.isNumber()) {
    int mod = modifiersV.asInt().unwrap();
    disp->updateModifierKeys(
      disp->getShiftKeyPressed() && !bit(mod,8),
      disp->getControlKeyPressed() && !bit(mod,2),
      disp->getAltKeyPressed() && !bit(mod,1),
      disp->getCommandKeyPressed() && !bit(mod,4)
    );
  }
  auto code = params["code"].asString();
  if (code.isOk()) {
    auto j = code.unwrap();
    geode::queueInMainThread([j,disp]{
    disp->dispatchKeyboardMSG(
      keyEnumForChar(std::tolower(j[0])),
      false, false
    );
    });
  }
  return emptyResponse();
}
$domainMethod(handleCharEvent) {
  auto disp = cocos2d::CCDirector::get()->getKeyboardDispatcher();

  auto modifiersV = params["modifiers"];
  bool shift = disp->getShiftKeyPressed();
  bool ctrl = disp->getControlKeyPressed();
  bool alt = disp->getAltKeyPressed();
  bool cmd = disp->getCommandKeyPressed();

  if (modifiersV.isNumber()) {
    int mod = modifiersV.asInt().unwrap();
    disp->updateModifierKeys(
      shift || !bit(mod,8),
      ctrl || !bit(mod,2),
      alt || !bit(mod,1),
      cmd || !bit(mod,4)
    );
  }

  auto code = params["code"].asString();
  geode::queueInMainThread([code,disp,shift,alt,ctrl,cmd]{
    if (code.isOk()) {
      for (char c : code.unwrap()) {
        disp->dispatchKeyboardMSG(
          keyEnumForChar(std::tolower(c)),
          false, false
        );
      }
    }
    disp->updateModifierKeys(shift,ctrl,alt,cmd);
  });

  return emptyResponse();
}
bool ignoreInputs = false;
$domainAsyncMethod(dispatchKeyEvent) {
  if (ignoreInputs) return finish(emptyResponse());
  auto type = params["type"].asString().unwrap();
  
  auto tsDiff = params["timestamp"].asInt().unwrapOr(0) - std::chrono::duration_cast<std::chrono::seconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();

  // async methods are run in a different thread because Task said so
  if (tsDiff > 0) std::this_thread::sleep_for(std::chrono::seconds(tsDiff));

  if (type == "keyDown") return finish(handleKeyDownEvent(params));
  if (type == "keyUp") return finish(handleKeyUpEvent(params));
  if (type == "char") return finish(handleCharEvent(params));

  return finish(errors::invalidParameter("Unknown key event type '"+type+"'"));
}
const std::string mouseButton[8] = {
  "left", "middle", "right", "back", "forward", "6", "7", "8"
};
int touchId = 0;
geode::cocos::CCArrayExt<cocos2d::CCTouch*> touches;
$domainAsyncMethod(dispatchMouseEvent) {
  if (ignoreInputs) return finish(emptyResponse());
  auto type = params["type"].asString().unwrap();
  
  auto tsDiff = params["timestamp"].asInt().unwrapOr(0) - std::chrono::duration_cast<std::chrono::seconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();

  // async methods are run in a different thread because Task said so
  if (tsDiff > 0) std::this_thread::sleep_for(std::chrono::seconds(tsDiff));

  if (type != "mouseWheel") {
    float x = params["x"].asDouble().unwrap();
    float y = params["y"].asDouble().unwrap();
    std::string button = params["button"].asString().unwrap();
    int buttonId = std::distance(mouseButton, std::find(mouseButton, mouseButton + 8, button));

    cocos2d::CCTouch* touch;
    if (type == "mousePress"){
      touch = new cocos2d::CCTouch();
      touches.push_back(touch);
      //touch->release(); // risky
    } else {
      // TODO: make mouse move actually do something (gd!lazer) by calling ccegl shit
      if (touches.size() == 0) return finish(emptyResponse());
      if (type == "mouseRelease") {
        touch = touches[touches.size()-1];
        touches.inner()->removeLastObject();
      }
      else touch = touches[touches.size()-1];
    }
    touch->setTouchInfo(touchId, x, y);
    auto set = cocos2d::CCSet::create();
    set->addObject(touch);
    geode::queueInMainThread([type,set,&finish]{
      if (type == "mousePress") {
        cocos2d::CCDirector::get()->getTouchDispatcher()->touchesBegan(set,nullptr);
      } else if (type == "mouseRelease") {
        cocos2d::CCDirector::get()->getTouchDispatcher()->touchesEnded(set,nullptr);
        //cocos2d::CCDirector::get()->getTouchDispatcher()->touchesCancelled(set,nullptr);
      } else {
        cocos2d::CCDirector::get()->getTouchDispatcher()->touchesMoved(set,nullptr);
      }
      finish(emptyResponse());
    });
  } else {
    int x = 0,y = 0;
    if (true) {
      auto x2 = params["deltaX"];
      if (!x2.isNumber()) x = x2.asDouble().unwrap();
      auto y2 = params["deltaY"];
      if (!y2.isNumber()) y = y2.asDouble().unwrap();
    }
    geode::queueInMainThread([x,y,&finish]{
      cocos2d::CCDirector::get()->getMouseDispatcher()->dispatchScrollMSG(x,y);
      finish(emptyResponse());
    });
  }
}
$domainMethod(setIgnoreInputEvents) {
  ignoreInputs = params["ignore"].asBool().unwrap();
  return emptyResponse();
}
$execute {
  auto p = Protocol::get();
  p->registerFunction("Input.dispatchKeyEvent", &dispatchKeyEvent);
  p->registerFunction("Input.dispatchMouseEvent", &dispatchMouseEvent, {"type","x","y","button"});
  p->registerFunction("Input.setIgnoreInputEvents", &setIgnoreInputEvents, {"input"});
}
//{"id":0,"method":"Input.dispatchMouseEvent","params":{"type":"mousePress","x":284,"y":160}}
