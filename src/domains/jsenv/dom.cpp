#include <Geode/Geode.hpp>
extern "C" void registerDOMNodeObject();
extern "C" void registerDOMSceneObject();

$on_mod(Loaded) {
  registerDOMNodeObject();
  registerDOMSceneObject();
}
