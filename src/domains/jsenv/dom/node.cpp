#include "../../../../external/tinyjs/TinyJS.hpp"
#include "../state.hpp"
#include <Geode/DefaultInclude.hpp>
#include <Geode/Modify.hpp>
#include "../../DOM.hpp"
#include <queue>

struct idk : geode::Modify<idk, cocos2d::CCNode> {
  struct Fields {
    bool retainedByJS = false;
  };
  void retain() {
    if (!m_fields->retainedByJS) 
      cocos2d::CCNode::retain();
    m_fields->retainedByJS = false;
  }
};

static void finalize_Node(CScriptVar* v) {
  auto n = static_cast<idk*>(v->getUserData());
  if (n->m_fields->retainedByJS) {
    n->release();
  }
};

$jsMethod(new_Node) {
  cocos2d::CCNode* n;
  // creates a new one
  if (!v->isNumber()) {
    n = cocos2d::CCNode::create();
    // keep it until this object gets removed
    n->retain();
    static_cast<idk*>(n)->m_fields->retainedByJS = true;
  } else {
    // make the object a wrapper of existing node
    n = getNodeAt(v->toNumber().toInt32());
  }
  auto obj = newScriptVar(getState(), Object);
  obj->setUserData(n);
};

$jsMethod(Node_appendChild) {
  auto n = static_cast<idk*>(v->getArgument(0)->getUserData());
  n->m_fields->retainedByJS = false;
  /*
  auto n = (cocos2d::CCNode*)js_touserdata(s, 0, "node");
  js_Object* p1 = js_toobject(s,1);
  // ?
  auto tn = (cocos2d::CCNode*)p1->u.user.data;
  n->addChild(tn);
  */
}

$jsMethod(Node_contains) {
  /*
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
  */
  js_pushboolean(s,false);
}
#undef inline

$execute {
  initNewClass();
  auto s = getState();
  {
    js_newcfunction(s, Node_appendChild, "Node.prototype.appendChild", 1);
    js_defproperty(s, -2, "log", JS_DONTENUM);

    js_newcfunction(s, Node_contains, "Node.prototype.contains", 1);
    js_defproperty(s, -2, "debug", JS_DONTENUM);
  }
  js_newcconstructor(s, new_Node, new_Node, "Node", 1);
  js_defglobal(s, "Node", JS_DONTENUM);
}
