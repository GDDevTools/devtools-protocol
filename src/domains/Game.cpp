#include "../WS.hpp"
#include <Geode/Geode.hpp>
#include <matjson.hpp>

$domainMethod(closeGame) {
  geode::queueInMainThread([]{
    geode::utils::game::exit();
  });
  return emptyResponse();
};
$domainMethod(crash) {
  geode::queueInMainThread([]{
    throw std::runtime_error("Manually initiated crash");
  });
  return emptyResponse();
};
$domainMethod(restart) {
  geode::queueInMainThread([]{
    geode::utils::game::restart();
  });
  return emptyResponse();
};
$domainMethod(getVersion) {
  auto modVer = geode::Mod::get()->getVersion();
  auto loaderVer = geode::Loader::get()->getVersion();
  auto gameVer = geode::Loader::get()->getGameVersion();
  using dude = std::vector<matjson::Value>;
  return geode::Ok(matjson::makeObject({
    {"protocolVersion", matjson::Value{dude{modVer.getMajor(), modVer.getMinor(), modVer.getPatch()}}},
    {"loaderVersion", matjson::Value{dude{loaderVer.getMajor(), loaderVer.getMinor(), loaderVer.getPatch()}}},
    {"gameVersion", gameVer},
  }));
};
enum class WindowState {
  normal,minimized,maximized,fullscreen
};
struct Bounds {
  intmax_t left;
  intmax_t top;
  intmax_t width;
  intmax_t height;
  WindowState windowState;
};

template<>
struct matjson::Serialize<WindowState> {
  static geode::Result<WindowState> fromJson(const matjson::Value& value) {
    auto sr = value.asString();
    if (sr.isErr()) return geode::Err("");
    auto s = sr.unwrap();
    WindowState status = WindowState::normal;
    if (s == "minimized") status = WindowState::minimized;
    if (s == "maximized") status = WindowState::maximized;
    if (s == "fullscreen") status = WindowState::fullscreen;
    return geode::Ok(status);
  }
  static matjson::Value toJson(const WindowState& e) {
    switch (e) {
      case WindowState::normal: return "normal";
      case WindowState::minimized: return "minimized";
      case WindowState::maximized: return "maximized";
      case WindowState::fullscreen: return "fullscreen";
    }
  }
};

template<>
struct matjson::Serialize<Bounds> {
  static geode::Result<Bounds> fromJson(const matjson::Value& value) {
    auto ws = matjson::Serialize<WindowState>::fromJson(value["windowState"]);
    if (ws.isErr()) return geode::Err("");
    return geode::Ok(Bounds{
      .left = value["left"].asInt().unwrapOr(0),
      .top = value["top"].asInt().unwrapOr(0),
      .width = value["width"].asInt().unwrapOr(0),
      .height = value["height"].asInt().unwrapOr(0),
      .windowState = ws.unwrap()
    });
  }
  static matjson::Value toJson(const Bounds& obj) {
    return matjson::makeObject({
      {"left", obj.left},
      {"top", obj.top},
      {"width", obj.width},
      {"height", obj.height},
      {"windowState", obj.windowState}
    });
  }
};

cocos2d::CCPoint winPos {0,0};
WindowState state = WindowState::normal;
WindowState oldState = WindowState::normal;
#ifdef GEODE_IS_WINDOWS
#include <Geode/modify/CCEGLView.hpp>
class $modify(cocos2d::CCEGLView) {
  void onGLFWWindowPosCallback(GLFWwindow* win, int x, int y) {
    cocos2d::CCEGLView::onGLFWWindowPosCallback(win, x, y);
    winPos.x = x;
    winPos.y = y;
  }
  void onGLFWWindowIconifyFunCallback(GLFWwindow* win, int iconified) {
    cocos2d::CCEGLView::onGLFWWindowIconifyFunCallback(win, iconified);
    oldState = state;
    state = (iconified) 
      ? WindowState::minimized
      : getIsFullscreen() ? WindowState::fullscreen : WindowState::normal;
  }
  void toggleFullScreen(bool full, bool borderless) {
    cocos2d::CCEGLView::toggleFullScreen(full, borderless,false);
    state = full ? WindowState::fullscreen : WindowState::normal;
  }
};
#endif

$domainMethod(getWindowBounds) {
  auto winSize = cocos2d::CCDirector::sharedDirector()->getOpenGLView()->getFrameSize();

  return geode::Ok(matjson::makeObject({
    {"bounds", Bounds{
      .left = (int)winPos.x,
      .top = (int)winPos.y,
      .width = (int)winSize.width,
      .height = (int)winSize.height,
      .windowState = state
    }}
  }));
}

$domainMethod(setWindowBounds) {
#ifdef GEODE_IS_WINDOWS
  auto gl = cocos2d::CCDirector::sharedDirector()->getOpenGLView();
  auto winSize = gl->getFrameSize();
  gl->setFrameSize(
    params["width"].asInt().unwrapOr(winSize.width), 
    params["height"].asInt().unwrapOr(winSize.height)
  );
  auto newState = matjson::Serialize<WindowState>::fromJson(params["windowState"]).unwrap();
  if (newState == WindowState::fullscreen) {
    gl->toggleFullScreen(true,false,false);
  }
  else {
    if (state == WindowState::fullscreen) {
      gl->toggleFullScreen(false,false,false);
    }
    if (newState == WindowState::minimized) {
      gl->iconify();
    }
    // TODO: morestuff
  }
#endif

  return emptyResponse();
}


$execute {
  auto p = Protocol::get();
  p->registerFunction("Game.close", &closeGame);
  p->registerFunction("Game.crash", &crash);
  p->registerFunction("Game.restart", &restart);
  p->registerFunction("Game.getVersion", &getVersion);
  p->registerFunction("Game.getWindowBounds", &getWindowBounds);
  p->registerFunction("Game.setWindowBounds", &setWindowBounds);
}
