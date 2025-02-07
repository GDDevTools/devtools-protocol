#include "element.hpp"
#include "domains/jsenv/state.hpp"

CScriptVarPtr elementPrototype;


#pragma region Properties
$jsMethod(Element_id_g) {
  validateContext<dom::CScriptVarElement>(v);
  v->setReturnVar(newScriptVar(
      v->getContext(),
      v->getThisPtr<dom::CScriptVarElementPtr>()->getNode()->getID()
  ));
}
$jsMethod(Element_id_s) {
  validateContext<dom::CScriptVarElement>(v);
  auto g = v->getArgument(1);
  if (g->isString())
    v->getThisPtr<dom::CScriptVarElementPtr>()->getNode()->setID(g->toString());
  v->setReturnVar(newScriptVarUndefined(v->getContext()));
}

$jsMethod(Element_className_g) {
  validateContext<dom::CScriptVarElement>(v);
  auto o = geode::cast::typeinfo_cast<cocos2d::CCString*>(
    v->getThisPtr<dom::CScriptVarElementPtr>()->getNode()->getUserObject(CSS_CLASSNAME_OBJ)
  );
  v->setReturnVar(newScriptVar(
      v->getContext(),
      o!=nullptr ? o->getCString() : ""
  ));
}
$jsMethod(Element_className_s) {
  validateContext<dom::CScriptVarElement>(v);
  auto g = v->getArgument(1);
  if (g->isString())
    v->getThisPtr<dom::CScriptVarElementPtr>()->getNode()->setUserObject(
      CSS_CLASSNAME_OBJ, cocos2d::CCString::create(g->toString())
    );
  v->setReturnVar(newScriptVarUndefined(v->getContext()));
}
#pragma endregion


#pragma region Functions
$jsMethod(Element_after) {
  int l = v->getArgumentsLength();
  auto node = validateContext<dom::CScriptVarElement>(v)->getNode();
  if (auto parent = node->getParent()) {
    for (int i = l-1; i>=0; i--) {
      auto elem = dynamic_cast<dom::CScriptVarElement*>(v->getArgument(i).getVar());
      if (elem==nullptr) continue;
      parent->insertAfter(elem->getNode(),node);
    }
  } else {
    throw newScriptVarCustomError(v->getContext(), "HierarchyRequestError", "No parent found for this node.");
  }

}
$jsMethod(Element_before) {
  int l = v->getArgumentsLength();
  auto node = validateContext<dom::CScriptVarElement>(v)->getNode();
  if (auto parent = node->getParent()) {
    for (int i = 0; i<l; i++) {
      auto elem = dynamic_cast<dom::CScriptVarElement*>(v->getArgument(i).getVar());
      if (elem==nullptr) continue;
      parent->insertBefore(elem->getNode(),node);
    }
  } else {
    throw newScriptVarCustomError(v->getContext(), "HierarchyRequestError", "No parent found for this node.");
  }
}

$jsMethod(Element_querySelector) {
  auto node = validateContext<dom::CScriptVarElement>(v)->getNode();
  auto query = v->getArgument("selector");
  if (query->isString()) {
    auto ret = node->querySelector(query->toString());
    if (ret==nullptr) return v->setReturnVar(newScriptVarNull(v->getContext()));
    v->setReturnVar(getAssociatedJSObject(ret));
  } else {
    v->getContext()->throwException(TypeError,"hi im kekcroc");
  }
}

$jsMethod(Element_remove) {
  validateContext<dom::CScriptVarElement>(v)->getNode()->removeFromParent();
}
#pragma endregion

namespace dom {
  CScriptVarElement::CScriptVarElement(CTinyJS *Context, cocos2d::CCNode* node)
  : CScriptVarNode(Context, node) {
    addChildOrReplace(TINYJS___PROTO___VAR, elementPrototype);

    addChild("id", newScriptVarAccessor(Context,Element_id_g,0,Element_id_s,0));
    addChild("className", newScriptVarAccessor(Context,Element_className_g,0,Element_className_s,0));
  };

  CScriptVarPtr CScriptVarElement::toString_CallBack(
    CScriptResult &execute, int radix
  ) {
    return newScriptVar("#element (TODO)");
  };
}; // namespace dom
$jsMethod(new_Element) {
  cocos2d::CCNode* n;
  // creates a new one
  n = cocos2d::CCNode::create();
  auto obj = newScriptVarElement(getState(), n);
  v->setReturnVar(obj);
};
extern "C" void registerDOMElementObject() {
  auto s = getState();
  auto elem = s->addNative("function Element()", new_Element,0,SCRIPTVARLINK_CONSTANT);
  elementPrototype = elem->findChild(TINYJS_PROTOTYPE_CLASS)->getVarPtr();
  elementPrototype->addChildOrReplace(TINYJS___PROTO___VAR, s->getRoot()->findChildByPath("Node.prototype"));
  {
    elementPrototype->addChild("after", newScriptVar(s, Element_after, 0, "after"));
    elementPrototype->addChild("before", newScriptVar(s, Element_before, 0, "before"));
    s->addNative("Element.prototype.querySelector(selector)", Element_querySelector);
    s->addNative("Element.prototype.remove()", Element_remove);
  }
  auto var = s->addNative("function Element.__constructor__()", new_Element, 0, SCRIPTVARLINK_CONSTANT);
  var->getFunctionData()->name = "Element";
}
