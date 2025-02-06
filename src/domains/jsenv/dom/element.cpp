#include "element.hpp"

CScriptVarPtr elementPrototype;

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
}

namespace dom {
  CScriptVarElement::CScriptVarElement(CTinyJS *Context, cocos2d::CCNode* node)
  : CScriptVarNode(Context, node) {
    addChildOrReplace(TINYJS___PROTO___VAR, elementPrototype);

    addChild("id", newScriptVarAccessor(Context,Element_id_g,0,Element_id_s,0));
  };
};
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
}
