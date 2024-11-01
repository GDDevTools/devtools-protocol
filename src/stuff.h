#include <Geode/cocos/base_nodes/CCNode.h>
using namespace cocos2d;

inline int nodeIdOf(CCNode* node) {
  return reinterpret_cast<intptr_t>(node);
}
