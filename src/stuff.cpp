#include "stuff.hpp"

int highestNodeId = 0;
/// a bunch of ub(?)
std::map<CCNode*, int> nodeIds;
std::queue<int> unused;
int nodeIdOf(CCNode* node) {
  if (unused.empty()) {
    return highestNodeId++;
  } else {
    int ret = unused.front();
    unused.pop();
    return ret;
  }
}

void unallocateNodeId(CCNode* node) {
  unused.push(nodeIds[node]);
  nodeIds.erase(node);
}
