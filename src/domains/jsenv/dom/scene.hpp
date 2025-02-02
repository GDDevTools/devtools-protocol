#include "node.hpp"

namespace dom {
  class CScriptVarNode;
  define_dummy_t(Scene);
  define_ScriptVarPtr_Type(Scene);
  class CScriptVarScene : public CScriptVarNode {
  protected:
    CScriptVarScene(CTinyJS *Context);
    CScriptVarScene(const CScriptVarScene &Copy) : CScriptVarNode(Copy) {} ///< Copy protected -> use clone for public
  public:

    /// std::string getParsableString(const std::string &indentString, const std::string &indent, uint32_t uniqueID, bool &hasRecursion);

    CScriptVarPtr toString_CallBack(CScriptResult &execute, int radix=0) override;
    friend define_newScriptVar_NamedFnc(Scene, CTinyJS*);

    cocos2d::CCNode* getNode() override;
  };
  inline define_newScriptVar_NamedFnc(Scene, CTinyJS *Context) { return new dom::CScriptVarScene(Context); } 
}