#include "../state.hpp"
#include <Geode/cocos/CCDirector.h>
#include <Geode/modify/CCDirector.hpp>
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
#ifdef GEODE_IS_DESKTOP
    CCDirector::sharedDirector()->getOpenGLView()->getIsFullscreen()
#else
    true
#endif
  ));
}

$jsMethod(Scene_lastModified_g) {
  v->setReturnVar(newScriptVarNumber(
    getState(),
    (double)lastmod.count()
  ));
}

#pragma endregion

#pragma region Functions

$jsMethod(new_Scene) {
  // creates a new one
  auto obj = v->newScriptVar(Object);
  obj->setUserData(CCDirector::sharedDirector()->getRunningScene());
  obj->addChildOrReplace(TINYJS___PROTO___VAR, getState()->getRoot()->findChildByPath("Scene.prototype"));
  v->setReturnVar(obj);
};

$jsMethod(Scene_requestFullscreen) {
#ifdef GEODE_IS_WINDOWS
  bool borderless = v->getArgumentsLength() != 0 ? v->getArgument(1)->toBoolean() : false;
  //CCDirector::get()->getOpenGLView()->toggleFullScreen(true, false, false);
  CCDirector::get()->pushScene(GraphicsReloadLayer::scene(
    CCDirector::get()->getLoadedTextureQuality(), 
    {0,0}, true, borderless, false, false
  ));
#endif
  v->setReturnVar(newScriptVarUndefined(getState()));
}
$jsMethod(Scene_exitFullscreen) {
#ifdef GEODE_IS_WINDOWS
  bool borderless = v->getArgumentsLength() != 0 ? v->getArgument(1)->toBoolean() : false;
  //CCDirector::get()->getOpenGLView()->toggleFullScreen(false, false, false);
  CCDirector::get()->pushScene(GraphicsReloadLayer::scene(
    CCDirector::get()->getLoadedTextureQuality(), 
    {0,0}, false, borderless, false, true
  ));
#endif
  v->setReturnVar(newScriptVarUndefined(getState()));
}

#pragma endregion

extern "C" void registerDOMSceneObject() {
  auto s = getState();
  auto node = s->addNative("function Scene()", new_Scene,0,SCRIPTVARLINK_CONSTANT);
  auto proto = node->findChild(TINYJS_PROTOTYPE_CLASS)->getVarPtr();
  proto->addChild(TINYJS_CONSTRUCTOR_VAR, node, SCRIPTVARLINK_BUILDINDEFAULT);
  proto->addChildOrReplace(TINYJS___PROTO___VAR, s->getRoot()->findChildByPath("Node.prototype"));
  {
    proto->addChild("fullscreenEnabled", newScriptVarAccessor(s, Scene_fullscreenEnabled_g, 0, nothing, 0));

    s->addNative("function Scene.prototype.requestFullscreen(scene, borderless)", Scene_requestFullscreen);
    s->addNative("function Scene.prototype.exitFullscreen(scene, borderless)", Scene_exitFullscreen);

  }
  s->addNative("function Scene.__constructor__()", new_Scene, (void*)1,SCRIPTVARLINK_CONSTANT);
  s->execute("var document = new Scene()");
}
