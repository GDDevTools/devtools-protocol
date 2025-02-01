#include "../state.hpp"
struct CCNodeJSHooks : geode::Modify<CCNodeJSHooks, cocos2d::CCNode> {
  struct Fields;
  void release();
  void setParent(cocos2d::CCNode* parent);
};
namespace dom {
  define_dummy_t(Node);
  define_ScriptVarPtr_Type(Node);
  class CScriptVarNode : public CScriptVarObject {
  protected:
    CScriptVarNode(CTinyJS *Context, cocos2d::CCNode* node);
    CScriptVarNode(const CScriptVarNode &Copy) : CScriptVarObject(Copy), node(Copy.node) {} ///< Copy protected -> use clone for public

    void signalRemoval();
  public:
    virtual ~CScriptVarNode();

    virtual std::string getParsableString(const std::string &indentString, const std::string &indent, uint32_t uniqueID, bool &hasRecursion);

    virtual CScriptVarPtr toString_CallBack(CScriptResult &execute, int radix=0);

    friend struct ::CCNodeJSHooks;

    cocos2d::CCNode* getNode() {return node;}
  private:
    cocos2d::CCNode* node = nullptr;
  };
  //inline define_newScriptVar_NamedFnc(Node, CTinyJS *Context, cocos2d::CCNode* node) { return new CScriptVarNode(Context, node); } 
}
//inline define_newScriptVar_NamedFnc(Node, CTinyJS *Context, cocos2d::CCNode* node) { return dom::newScriptVarNode(Context, node); } 
