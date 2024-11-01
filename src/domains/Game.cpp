#include "../WS.hpp"
#include <Geode/Geode.hpp>
#include <matjson.hpp>

namespace nameCollision {
$domainMethod(close) {
  geode::queueInMainThread([]{
    geode::utils::game::exit();
  });
  return geode::Ok(matjson::Object{});
};
}
$domainMethod(crash) {
  geode::queueInMainThread([]{
    throw std::runtime_error("Manually initiated crash");
  });
  return geode::Ok(matjson::Object{});
};
$domainMethod(restart) {
  geode::queueInMainThread([]{
    geode::utils::game::restart();
  });
  return geode::Ok(matjson::Object{});
};
$domainMethod(getVersion) {
  auto modVer = geode::Mod::get()->getVersion();
  auto loaderVer = geode::Loader::get()->getVersion();
  auto gameVer = geode::Loader::get()->getGameVersion();
  return geode::Ok(matjson::Object{
    {"protocolVersion", matjson::Array{modVer.getMajor(), modVer.getMinor(), modVer.getPatch()}},
    {"loaderVersion", matjson::Array{loaderVer.getMajor(), loaderVer.getMinor(), loaderVer.getPatch()}},
    {"gameVersion", gameVer},
  });
}
enum class WindowState {
  normal,minimized,maximized,fullscreen
};
struct Bounds {
  int left;
  int top;
  int width;
  int height;
  WindowState windowState;
};

template<>
struct matjson::Serialize<WindowState> {
  static WindowState from_json(const matjson::Value& value) {
    auto s = value.as_string();
    if (s == "normal") return WindowState::normal;
    if (s == "minimized") return WindowState::minimized;
    if (s == "maximized") return WindowState::maximized;
    if (s == "fullscreen") return WindowState::fullscreen;
    return WindowState::normal;
  }
  static matjson::Value to_json(const WindowState& e) {
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
  static Bounds from_json(const matjson::Value& value) {
    return Bounds{
      .left = value["left"].is_number() ? value["left"].as_int() : 0,
      .top = value["top"].is_number() ? value["top"].as_int() : 0,
      .width = value["width"].as_int() ? value["width"].as_int() : 0,
      .height = value["height"].as_int() ? value["height"].as_int() : 0,
      .windowState = matjson::Serialize<WindowState>::from_json(value["windowState"])
    };
  }
  static matjson::Value to_json(const Bounds& obj) {
    return matjson::Object{
      {"left", obj.left},
      {"top", obj.top},
      {"width", obj.width},
      {"height", obj.height},
      {"windowState", obj.windowState}
    };
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
    winPos.y = y
  }
  void onGLFWWindowIconifyFunCallback(GLFWwindow* win, int iconified) {
    cocos2d::CCEGLView::onGLFWWindowIconifyFunCallback(win, iconified);
    oldState = state;
    state = (iconified) 
      ? WindowState::minimized
      : getIsFullscreen() ? WindowState::fullscreen : WindowState::normal;
  }
  void toggleFullScreen(bool full, bool borderless) {
    cocos2d::CCEGLView::toggleFullScreen(full, borderless);
    state = full ? WindowState::fullscreen : WindowState::normal;
  }
}
#endif

$domainMethod(getWindowBounds) {
  auto winSize = cocos2d::CCDirector::sharedDirector()->getOpenGLView()->getFrameSize();

  return geode::Ok(matjson::Value{Bounds{
    .left = (int)winPos.x,
    .top = (int)winPos.y,
    .width = (int)winSize.width,
    .height = (int)winSize.height,
    .windowState = state
  }}.as_object());
}

$domainMethod(setWindowBounds) {
#ifndef GEODE_IS_MOBILE
  auto gl = cocos2d::CCDirector::sharedDirector()->getOpenGLView();
  auto winSize = gl->getFrameSize();
  gl->setFrameSize(
    params["width"].is_null() ? winSize.width : params["width"].as_int(), 
    params["height"].is_null() ? winSize.height : params["height"].as_int()
  );
  auto newState = matjson::Serialize<WindowState>::from_json(params["windowState"]);
  if (newState == WindowState::fullscreen) {
    gl->toggleFullScreen(true);
  }
  else {
    if (state == WindowState::fullscreen) {
      gl->toggleFullScreen(false);
    }
    if (newState == WindowState::minimized) {
      gl->iconify();
    }
    // TODO: morestuff
  }
#endif

  return geode::Ok(matjson::Object{});
}


$execute {
  auto p = Protocol::get();
  p->registerFunction("Game.close", &nameCollision::close);
  p->registerFunction("Game.crash", &crash);
  p->registerFunction("Game.restart", &restart);
  p->registerFunction("Game.getVersion", &getVersion);
  p->registerFunction("Game.getWindowBounds", &getWindowBounds);
  p->registerFunction("Game.setWindowBounds", &setWindowBounds);
}
