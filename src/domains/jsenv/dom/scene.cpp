#include "../state.hpp"
#include <Geode/cocos/CCDirector.h>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/Modify.hpp>
#include <chrono>

std::chrono::milliseconds lastmod;
using namespace cocos2d;
struct amoung : geode::Modify<amoung, CCDirector> {
    void lastModified() {
        lastmod = std::chrono::duration_cast<decltype(lastmod)>(
            std::chrono::system_clock::now().time_since_epoch()
        );
    }
    bool pushScene(CCScene *pScene) {
        lastModified();
        return CCDirector::pushScene(pScene);
    }
    void popScene(void) {
        lastModified();
        return CCDirector::popScene();
    }
    bool replaceScene(CCScene *pScene) {
        lastModified();
        return CCDirector::replaceScene(pScene);
    }
};

#pragma region Properties
$jsMethod(Scene_fullscreenEnabled_g) {
    v->setReturnVar(newScriptVarBool(
        getState(),
        CCDirector::sharedDirector()->getOpenGLView()->getIsFullscreen()
    ));
}
#pragma endregion

$jsMethod(new_Scene) {
  // creates a new one
  auto obj = newScriptVar(getState(), Object);
  obj->setUserData(CCDirector::sharedDirector()->getRunningScene());
};

extern "C" void registerDOMSceneObject() {
    auto s = getState();
    auto node = s->addNative("function Scene()", new_Scene,0,SCRIPTVARLINK_CONSTANT);
    auto proto = node->findChild(TINYJS_PROTOTYPE_CLASS)->getVarPtr();
    proto->addChild(TINYJS_CONSTRUCTOR_VAR, node);
}