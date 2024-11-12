#include "../state.hpp"
#include "../../../external/mujs/jsi.h"
#include <Geode/DefaultInclude.hpp>
#include "../../DOM.hpp"
#include <queue>
#undef inline

$jsMethod(new_Node) {
  cocos2d::CCNode* n;
  // creates a new one
  if (!js_isnumber(s, 1)) {
    n = cocos2d::CCNode::create();
    // keep it until this object gets removed
    n->retain();
  } else {
    // make the object a wrapper of existing node
    n = getNodeAt(js_tonumber(s,1));
  }
  js_currentfunction(s);
  js_getproperty(s, -1, "prototype");
  js_newuserdata(s, "node", n, NULL);
}

$jsMethod(Node_appendChild) {
  auto n = (cocos2d::CCNode*)js_touserdata(s, 0, "node");
  auto p1 = js_toobject(s,1);
  // ?
  auto tn = (cocos2d::CCNode*)p1->u.user.data;
  n->addChild(tn);
  js_pushundefined(s);
}

$jsMethod(Node_contains) {
  auto n = (cocos2d::CCNode*)js_touserdata(s, 0, "node");
  if (js_isnull(s,1)) {js_pushboolean(s,false);return;}
  auto p1 = js_toobject(s,1);
  // ?
  auto tn = (cocos2d::CCNode*)p1->u.user.data;
  // idk if this check is ever gonna be true
  // idk about pointers
  if (n==tn) {js_pushboolean(s,true);return;}
  if (tn->getChildrenCount()==0) {js_pushboolean(s,false);return;}

  std::queue<cocos2d::CCArray*> q;
  q.push(n->getChildren());
  while (!q.empty()) {
    for (auto* c : geode::cocos::CCArrayExt<cocos2d::CCNode*>(q.front())) {
      if (c == tn) {js_pushboolean(s,true);return;}
      else {
        if (c->getChildrenCount()!=0) q.push(c->getChildren());
      }
    }
    q.pop();
  }
  js_pushboolean(s,false);
}

$execute {
  initNewClass();
  auto s = getState();
  js_newcconstructor(s, new_Node, new_Node, "Node", 1);
  js_defglobal(s, "Node", JS_DONTENUM);
}