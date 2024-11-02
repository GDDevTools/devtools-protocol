#include "../WS.hpp"
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
  return geode::Ok(matjson::Object{});
}
$domainMethod(dispatchKeyEvent) {
  auto type = params["type"].as_string();
  if (type == "keyUp") return handleKeyDownEvent(params);

  return errors::invalidParameter("Unknown key event type '"+type+"'");
}
