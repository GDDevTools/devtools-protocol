#include "../state.hpp"
#include "node.hpp"
#include <Geode/cocos/include/cocos2d.h>
//CScriptVar* getAssociatedJSObject(cocos2d::CCNode* node);
namespace dom {
  define_dummy_t(Element);
  define_ScriptVarPtr_Type(Element);
  class CScriptVarElement : public CScriptVarNode {
  protected:
    CScriptVarElement(CTinyJS *Context, cocos2d::CCNode* node);
    CScriptVarElement(const CScriptVarNode &Copy) : CScriptVarNode(Copy) {} ///< Copy protected -> use clone for public

  public:
    void signalRemoval();
    virtual ~CScriptVarElement();

    virtual std::string getParsableString(const std::string &indentString, const std::string &indent, uint32_t uniqueID, bool &hasRecursion);

    virtual CScriptVarPtr toString_CallBack(CScriptResult &execute, int radix=0);

    friend struct CCNodeJSHooks;
    friend define_newScriptVar_NamedFnc(Element, CTinyJS*, cocos2d::CCNode*);
  private:
    std::string className;

  };
  inline define_newScriptVar_NamedFnc(Element, CTinyJS *Context, cocos2d::CCNode* node) { return new CScriptVarElement(Context, node); } 
}
inline define_newScriptVar_NamedFnc(Element, CTinyJS *Context, cocos2d::CCNode* node) { return dom::newScriptVarElement(Context, node); } 
