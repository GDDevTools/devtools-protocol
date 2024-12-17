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

static void returnNode(CFunctionsScopePtr const& v, cocos2d::CCNode* n) {
  auto nodeobj = getState()->evaluateComplex("new Node()");
  static_cast<cocos2d::CCNode*>(nodeobj->getVarPtr()->getUserData())->release();
  nodeobj->getVarPtr()->setUserData(n);
  v->setUserData(nodeobj->getVarPtr().getVar());
}
//////
/// Properties
//////

$jsMethod(Node_isConnected_g) {
  v->setReturnVar(
    newScriptVarBool(
      getState(),
      static_cast<cocos2d::CCNode*>(
        v->getArgument("this")->getUserData()
      )->isRunning() // this is usually the way to determine if its connected to the scene
                     // but you know you can never trust a modder
    )
  );
}


$jsMethod(Node_firstChild_g) {
  auto node = static_cast<cocos2d::CCNode*>(v->getArgument("this")->getUserData());
  auto cl = node->getChildren();
  if (cl == nullptr || cl->count()==0) goto retnull; 
  returnNode(v, static_cast<cocos2d::CCNode*>(cl->firstObject())); return;
retnull:
  v->setReturnVar(newScriptVarNull(getState()));
  return;
}

$jsMethod(Node_lastChild_g) {
  auto node = static_cast<cocos2d::CCNode*>(v->getArgument("this")->getUserData());
  auto cl = node->getChildren();
  if (cl == nullptr || cl->count()==0) goto retnull; 
  returnNode(v, static_cast<cocos2d::CCNode*>(cl->lastObject())); return;
retnull:
  v->setReturnVar(newScriptVarNull(getState()));
  return;
}


$jsMethod(Node_nextSibling_g) {
  auto node = static_cast<cocos2d::CCNode*>(v->getArgument("this")->getUserData());
  if (auto p = node->getParent()) {
    auto cl = p->getChildren();
    auto i = cl->indexOfObject(node);
    if (i == cl->count()-1) goto retnull; 
    returnNode(v, static_cast<cocos2d::CCNode*>(cl->objectAtIndex(i+1))); return;
  }
retnull:
  v->setReturnVar(newScriptVarNull(getState()));
  return;
}

$jsMethod(Node_previousSibling_g) {
  auto node = static_cast<cocos2d::CCNode*>(v->getArgument("this")->getUserData());
  if (auto p = node->getParent()) {
    auto cl = p->getChildren();
    auto i = cl->indexOfObject(node);
    if (i == 0) goto retnull; 
    returnNode(v, static_cast<cocos2d::CCNode*>(cl->objectAtIndex(i-1))); return;
  }
retnull:
  v->setReturnVar(newScriptVarNull(getState()));
  return;
}


$jsMethod(Node_textContent_g) {
  auto node = static_cast<cocos2d::CCNode*>(v->getArgument("this")->getUserData());
  cocos2d::CCLabelProtocol* textNode = geode::cast::typeinfo_cast<cocos2d::CCLabelTTF*>(node);
  if (textNode == nullptr) 
    textNode = geode::cast::typeinfo_cast<cocos2d::CCLabelBMFont*>(node);
  if (textNode == nullptr) goto retnull;
  v->setReturnVar(newScriptVar(getState(), textNode->getString()));
retnull:
  v->setReturnVar(newScriptVarNull(getState()));
  return;
}
$jsMethod(Node_textContent_s) {
  if (!v->getArgument(1)->isString()) goto ret;
  {
    auto node = static_cast<cocos2d::CCNode*>(v->getArgument("this")->getUserData());
    cocos2d::CCLabelProtocol* textNode = geode::cast::typeinfo_cast<cocos2d::CCLabelTTF*>(node);
    if (textNode == nullptr) 
      textNode = geode::cast::typeinfo_cast<cocos2d::CCLabelBMFont*>(node);
    if (textNode == nullptr) goto ret;
    textNode->setString(v->getArgument(1)->toString().c_str());
  }
ret:
  v->setReturnVar(newScriptVarUndefined(getState()));
  return;
}

//////
/// Functions
//////

$jsMethod(new_Node) {
  cocos2d::CCNode* n;
  // creates a new one
  n = cocos2d::CCNode::create();
  // keep it until this object gets removed
  n->retain();
  static_cast<idk*>(n)->m_fields->retainedByJS = true;
  auto obj = newScriptVar(getState(), Object);
  obj->setUserData(n);
};

$jsMethod(Node_appendChild) {
  auto n = static_cast<idk*>(v->getArgument("node")->getUserData());
  static_cast<idk*>(v->getArgument("this")->getUserData())->addChild(n);
  v->setReturnVar(newScriptVarUndefined(getState()));
}
$jsMethod(Node_insertBefore) {
  auto n = static_cast<idk*>(v->getArgument("node")->getUserData());
  auto rnv = v->getArgument("refNode");
  if (rnv->isNull())
    static_cast<idk*>(v->getArgument("this")->getUserData())->addChild(n);
  else if (rnv->isObject())
    static_cast<idk*>(
      v->getArgument("this")->getUserData()
    )->insertBefore(
      n, 
      static_cast<idk*>(rnv->getUserData())
    );

  v->setReturnVar(newScriptVarUndefined(getState()));
}

$jsMethod(Node_contains) {
  auto nodeVar = v->getArgument("node");
  if (nodeVar->isUndefined()) {
    v->throwError(ERROR_TYPES::Error, "node is required vro");
    return;
  }
  if (nodeVar->isNull()) {
    v->setReturnVar(newScriptVarBool(getState(), false));
    return;
  }
  auto thisNode = static_cast<idk*>(v->getArgument("this")->getUserData());
  auto targetNode = static_cast<idk*>(nodeVar->getUserData());
  if (thisNode == targetNode) {
    v->setReturnVar(newScriptVarBool(getState(), true));
    return;
  }

  std::queue<cocos2d::CCNode*> q;
  q.push(thisNode);
  while (!q.empty()) {
    auto cNode = q.front();

    for (auto c : geode::cocos::CCArrayExt<cocos2d::CCNode*>(cNode->getChildren())) {
      if (c == targetNode) {
        v->setReturnVar(newScriptVarBool(getState(), true));
        return;
      }
      q.push(c);
    }

    q.pop();
  }

  v->setReturnVar(newScriptVarBool(getState(), false));
}
$jsMethod(Node_getRootNode) {
  auto thisNode = static_cast<idk*>(v->getArgument("this")->getUserData());

  cocos2d::CCNode* lp = thisNode;
  cocos2d::CCNode* p = thisNode->getParent();
  while (p) {
    lp = p;
    p = p->getParent();
  }

  returnNode(v, lp); return;
}
$jsMethod(Node_hasChildNodes) {
  v->setReturnVar(
    newScriptVarBool(
      getState(),
      static_cast<cocos2d::CCNode*>(
        v->getArgument("this")->getUserData()
      )->getChildrenCount() != 0
    )
  );
}
#undef inline

$execute {
  auto s = getState();
  auto node = s->addNative("function Node()", new_Node,0,SCRIPTVARLINK_CONSTANT);
  auto proto = node->findChild(TINYJS_PROTOTYPE_CLASS)->getVarPtr();
  proto->addChild(TINYJS_CONSTRUCTOR_VAR, node);
  {
    proto->addChild("firstChild", newScriptVarAccessor(s, Node_firstChild_g,0,&nothing,0));
    proto->addChild("isConnected", newScriptVarAccessor(s, Node_isConnected_g,0,&nothing,0));
    proto->addChild("lastChild", newScriptVarAccessor(s, Node_lastChild_g,0,&nothing,0));
    proto->addChild("nextSibling", newScriptVarAccessor(s, Node_nextSibling_g,0,&nothing,0));
    proto->addChild("previousSibling", newScriptVarAccessor(s, Node_previousSibling_g,0,&nothing,0));
    proto->addChild("textContent", newScriptVarAccessor(s, Node_textContent_g,0,Node_textContent_s,0));

    s->addNative("function Node.prototype.appendChild(node)", Node_appendChild);
    s->addNative("function Node.prototype.contains(node)", Node_contains);
    s->addNative("function Node.prototype.getRootNode(node)", Node_getRootNode);
    s->addNative("function Node.prototype.hasChildNodes(node)", Node_hasChildNodes);
    s->addNative("function Node.prototype.insertBefore(node, refNode)", Node_insertBefore);
  }
}
