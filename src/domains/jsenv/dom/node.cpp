/// Prepare for the completely unreadable source code!


//#include "../../../external/tinyjs/TinyJS.hpp"
#include "../state.hpp"
#include "domains/external/tinyjs/TinyJS.hpp"
#include <Geode/DefaultInclude.hpp>
#include <queue>
#include "node.hpp"

#include <Geode/modify/CCNode.hpp>
struct CCNodeJSHooks : geode::Modify<CCNodeJSHooks, cocos2d::CCNode> {
  struct Fields {
    bool jsSetup = false;
    dom::CScriptVarNodePtr associatedJSObject;
  };
  void release() {
    if (m_uReference == 1) {
      m_fields->associatedJSObject->signalRemoval();
    }
    cocos2d::CCNode::release();
  }
  void setParent(cocos2d::CCNode* parent);
};
void CCNodeJSHooks::setParent(cocos2d::CCNode* parent) {
  cocos2d::CCNode::setParent(parent);
  if (!m_fields->jsSetup) {
    m_fields->associatedJSObject = dom::newScriptVarNode(getState(), this);
    m_fields->jsSetup = true;
  }
}
CScriptVar* getAssociatedJSObject(cocos2d::CCNode* node) {
  return static_cast<CCNodeJSHooks*>(node)->m_fields->associatedJSObject.getVar();
};

/*
! ===========================================================================================
static void finalize_Node(CScriptVar* v) {
  auto n = static_cast<CCNodeJSHooks*>(v->getUserData());
  if (n->m_fields->retainedByJS) {
    n->release();
  }
};
*/

static void returnNode(CFunctionsScopePtr const& v, cocos2d::CCNode* n) {
  v->setReturnVar(static_cast<CCNodeJSHooks*>(n)->m_fields->associatedJSObject);
}
//////
/// Properties
//////
#pragma region Properties

$jsMethod(Node_childList_g) {
  auto arr = newScriptVar(getState(), Array);
  int idx = 0;
  for (auto* c : geode::cocos::CCArrayExt<CCNodeJSHooks*>(
    static_cast<dom::CScriptVarNodePtr>(v->findChild("this")->getVarPtr())
    ->getNode()->getChildren()
  )) {
    arr->setArrayIndex(idx,c->m_fields->associatedJSObject);
    idx++;
  }
  v->setReturnVar(arr);
}

$jsMethod(Node_isConnected_g) {
  v->setReturnVar(
    newScriptVarBool(
      getState(),
      static_cast<dom::CScriptVarNodePtr>(
        v->findChild("this")->getVarPtr()
      )->getNode()->isRunning() // this is usually the way to determine if its connected to the scene
                     // but you know you can never trust a modder
    )
  );
}
$jsMethod(Node_isSameNode) {
  v->setReturnVar(
    newScriptVarBool(
      getState(),

      // check if these 2 variables points to the same address
      v->findChild("this")->getVarPtr()->getUserData()
      ==
      v->getArgument("node")->getUserData()
    )
  );
}

$jsMethod(Node_firstChild_g) {
  auto node = static_cast<dom::CScriptVarNodePtr>(v->findChild("this")->getVarPtr())->getNode();
  auto cl = node->getChildren();
  if (cl == nullptr || cl->count()==0) goto retnull; 
  returnNode(v, static_cast<cocos2d::CCNode*>(cl->firstObject())); return;
retnull:
  v->setReturnVar(newScriptVarNull(getState()));
  return;
}

$jsMethod(Node_lastChild_g) {
  auto node = static_cast<dom::CScriptVarNodePtr>(v->findChild("this")->getVarPtr())->getNode();
  auto cl = node->getChildren();
  if (cl == nullptr || cl->count()==0) goto retnull; 
  returnNode(v, static_cast<cocos2d::CCNode*>(cl->lastObject())); return;
retnull:
  v->setReturnVar(newScriptVarNull(getState()));
  return;
}


$jsMethod(Node_nextSibling_g) {
  auto node = static_cast<dom::CScriptVarNodePtr>(v->findChild("this")->getVarPtr())->getNode();
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
  auto node = static_cast<dom::CScriptVarNodePtr>(v->findChild("this")->getVarPtr())->getNode();
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
  auto node = static_cast<dom::CScriptVarNodePtr>(v->findChild("this")->getVarPtr())->getNode();
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
    auto node = static_cast<dom::CScriptVarNodePtr>(v->findChild("this")->getVarPtr())->getNode();
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
#pragma endregion

//////
/// Functions
//////
#pragma region Functions
$jsMethod(new_Node) {
  cocos2d::CCNode* n;
  // creates a new one
  n = cocos2d::CCNode::create();
  auto obj = newScriptVar(getState(), Object,getState()->getRoot()->findChildByPath("Node.prototype"));
  obj->setUserData(n);
};
#define $getNodeOfVariable2(n, var) \
  cocos2d::CCNode* n = nullptr; \
  { \
    auto vava = dynamic_cast<dom::CScriptVarNode*>(var.getVar()); \
    n = vava != nullptr \
      ? vava->getNode()  \
      : nullptr; \
    if (!n) { \
      v->throwError(ReferenceError, "Not a valid Node object."); \
      return; \
    } \
  } 
#define $getNodeOfVariable(var) $getNodeOfVariable2(n, var)

$jsMethod(Node_appendChild) {
  validateContext<dom::CScriptVarNode>(v);
  $getNodeOfVariable(v->getArgument("node"));
  static_cast<dom::CScriptVarNodePtr>(v->findChild("this")->getVarPtr())->getNode()->addChild(n);
  v->setReturnVar(newScriptVarUndefined(getState()));
}
$jsMethod(Node_removeChild) {
  validateContext<dom::CScriptVarNode>(v);
  $getNodeOfVariable(v->getArgument("node"));
  static_cast<dom::CScriptVarNodePtr>(v->findChild("this")->getVarPtr())->getNode()->removeChild(n);
  v->setReturnVar(newScriptVarUndefined(getState()));
}
$jsMethod(Node_replaceChild) {
  validateContext<dom::CScriptVarNode>(v);
  $getNodeOfVariable2(oldNode, v->getArgument("oldChild"));
  $getNodeOfVariable2(newNode, v->getArgument("newChild"));
  auto thisNode = static_cast<dom::CScriptVarNodePtr>(v->findChild("this")->getVarPtr())->getNode();
  auto children = thisNode->getChildren();
  int insertIndex = -1;
  if (children) {
    insertIndex = children->count()-1;
    if (children->containsObject(oldNode)) {
      insertIndex = children->indexOfObject(oldNode);
      children->removeObjectAtIndex(insertIndex);
    }
  }
  if (insertIndex == -1) {
    thisNode->addChild(newNode);
  } else {
    children->insertObject(newNode,insertIndex);
  }
  v->setReturnVar(newScriptVarUndefined(getState()));
}
$jsMethod(Node_insertBefore) {
  validateContext<dom::CScriptVarNode>(v);
  $getNodeOfVariable(v->getArgument("node"));
  auto rnv = v->getArgument("refNode");
  if (rnv->isNull())
    static_cast<dom::CScriptVarNodePtr>(v->findChild("this")->getVarPtr())->getNode()->addChild(n);
  else if (rnv->isObject())
    static_cast<dom::CScriptVarNodePtr>(
      v->findChild("this")->getVarPtr()
    )->getNode()->insertBefore(
      n, 
      static_cast<CCNodeJSHooks*>(rnv->getUserData())
    );

  v->setReturnVar(newScriptVarUndefined(getState()));
}

$jsMethod(Node_contains) {
  validateContext<dom::CScriptVarNode>(v);
  auto nodeVar = v->getArgument("node");
  if (nodeVar->isUndefined()) {
    v->throwError(ERROR_TYPES::Error, "node is required vro");
    return;
  }
  if (nodeVar->isNull()) {
    v->setReturnVar(newScriptVarBool(getState(), false));
    return;
  }
  auto thisNode = static_cast<dom::CScriptVarNodePtr>(v->findChild("this")->getVarPtr())->getNode();
  auto targetNode = static_cast<CCNodeJSHooks*>(nodeVar->getUserData());
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
  validateContext<dom::CScriptVarNode>(v);
  auto thisNode = static_cast<dom::CScriptVarNodePtr>(v->findChild("this")->getVarPtr())->getNode();

  cocos2d::CCNode* lp = thisNode;
  cocos2d::CCNode* p = thisNode->getParent();
  while (p) {
    lp = p;
    p = p->getParent();
  }

  returnNode(v, lp); return;
}
$jsMethod(Node_getChildById) {
  validateContext<dom::CScriptVarNode>(v);
  auto thisNode = static_cast<dom::CScriptVarNodePtr>(v->findChild("this")->getVarPtr());
  auto nodeId = v->getArgument("id")->toString();

  auto node = thisNode->getNode()->getChildByID(nodeId);
  if (node==nullptr) {
    v->throwError(Error, "No such child with ID \""+nodeId+"\"");
    return;
  }

  return returnNode(v, node);
}

$jsMethod(Node_hasChildNodes) {
  validateContext<dom::CScriptVarNode>(v);
  v->setReturnVar(
    newScriptVarBool(
      getState(),
      static_cast<dom::CScriptVarNodePtr>(
        v->findChild("this")->getVarPtr()
      )->getNode()->getChildrenCount() != 0
    )
  );
}
#pragma endregion

#pragma region CScriptVarNode
CScriptVarPtr nodePrototype;

namespace dom {
  CScriptVarNode::CScriptVarNode(CTinyJS *Context, cocos2d::CCNode* node) 
  : CScriptVarObject(Context, nodePrototype), node(node) {
    addChild("childList", newScriptVarAccessor(Context, Node_childList_g,0,&nothing,0));
    addChild("firstChild", newScriptVarAccessor(Context, Node_firstChild_g,0,&nothing,0));
    addChild("isConnected", newScriptVarAccessor(Context, Node_isConnected_g,0,&nothing,0));
    addChild("lastChild", newScriptVarAccessor(Context, Node_lastChild_g,0,&nothing,0));
    addChild("nextSibling", newScriptVarAccessor(Context, Node_nextSibling_g,0,&nothing,0));
    addChild("previousSibling", newScriptVarAccessor(Context, Node_previousSibling_g,0,&nothing,0));
    addChild("textContent", newScriptVarAccessor(Context, Node_textContent_g,0,Node_textContent_s,0));
  }

  CScriptVarPtr CScriptVarNode::toString_CallBack(CScriptResult &execute, int radix) {
    return ::newScriptVar(context, "node");
  };
  std::string CScriptVarNode::getParsableString(const std::string &indentString, const std::string &indent, uint32_t uniqueID, bool &hasRecursion) {
    return "idk sorry";
  };

  void CScriptVarNode::signalRemoval() {
    node = nullptr;
  }
  CScriptVarNode::~CScriptVarNode() {
    // ? should i remove the node if this got destroyed ?
  };

};
#pragma endregion

extern "C" void registerDOMNodeObject() {
  auto s = getState();
  auto node = s->addNative("function Node()", new_Node,0,SCRIPTVARLINK_CONSTANT);
  nodePrototype = node->findChild(TINYJS_PROTOTYPE_CLASS)->getVarPtr().getVar();
  nodePrototype->addChild(TINYJS_CONSTRUCTOR_VAR, node, SCRIPTVARLINK_BUILDINDEFAULT);
  {
    /*
    nodePrototype->addChild("childList", newScriptVarAccessor(s, Node_childList_g,0,&nothing,0));
    nodePrototype->addChild("firstChild", newScriptVarAccessor(s, Node_firstChild_g,0,&nothing,0));
    nodePrototype->addChild("isConnected", newScriptVarAccessor(s, Node_isConnected_g,0,&nothing,0));
    nodePrototype->addChild("lastChild", newScriptVarAccessor(s, Node_lastChild_g,0,&nothing,0));
    nodePrototype->addChild("nextSibling", newScriptVarAccessor(s, Node_nextSibling_g,0,&nothing,0));
    nodePrototype->addChild("previousSibling", newScriptVarAccessor(s, Node_previousSibling_g,0,&nothing,0));
    nodePrototype->addChild("textContent", newScriptVarAccessor(s, Node_textContent_g,0,Node_textContent_s,0));
    */
    s->addNative("function Node.prototype.appendChild(node)", Node_appendChild);
    s->addNative("function Node.prototype.contains(node)", Node_contains);
    s->addNative("function Node.prototype.getChildById(node, id)", Node_getChildById);
    s->addNative("function Node.prototype.getRootNode(node)", Node_getRootNode);
    s->addNative("function Node.prototype.hasChildNodes(node)", Node_hasChildNodes);
    s->addNative("function Node.prototype.insertBefore(node, refNode)", Node_insertBefore);
    s->addNative("function Node.prototype.isSameNode(node)", Node_isSameNode);
    s->addNative("function Node.prototype.removeChild(node)", Node_removeChild);
    s->addNative("function Node.prototype.replaceChild(node, newChild, oldChild)", Node_replaceChild);
  }
  auto var = s->addNative("function Node.__constructor__()", new_Node, 0, SCRIPTVARLINK_CONSTANT);
  var->getFunctionData()->name = "Node";
}