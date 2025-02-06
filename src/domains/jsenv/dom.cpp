#include <Geode/Geode.hpp>
extern "C" void registerDOMNodeObject();
extern "C" void registerDOMSceneObject();
extern "C" void registerDOMElementObject();

$on_mod(Loaded) {
  registerDOMNodeObject();
  registerDOMSceneObject();
  registerDOMElementObject();
}
