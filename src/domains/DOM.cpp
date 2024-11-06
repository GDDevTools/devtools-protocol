#include <Geode/utils/cocos.hpp>
#include "../WS.hpp"
#include <matjson.hpp>
#include <Geode/Modify.hpp>
#include <queue>

#include "../stuff.hpp"

bool DOMDomainDisabled = true;

inline void fireDOMEvent(std::string eventName, matjson::Object const& content = {}) {
  fireEvent("DOM."+eventName, content);
};

static std::unordered_map<int, CCNode*> s_nodes;

CCNode* getNodeAt(int id) {
  if (id < 0) return nullptr;
  if (s_nodes.find(id)!=s_nodes.end()) {
    return s_nodes[id];
  } else {
    return nullptr;
  }
}

int highestNodeId = 0;

#include <Geode/modify/CCNode.hpp>
struct CCNode2 : geode::Modify<CCNode2, CCNode> {
  struct Fields {
    // a copy of the thing from GeodeNodeMetadata
    std::unordered_map<std::string, CCObject*> m_userObjects;
    int depth = 0;
    int nodeId = 0;
  };
  void constructor() {
    cocos2d::CCNode();
    m_fields->nodeId = ++highestNodeId;
  }
  void retain() {
    if (retainCount()==0) s_nodes[m_fields->nodeId] = this;
    CCNode::retain();
  }
  void release() {
    if (retainCount()==1) {
      s_nodes.erase(m_fields->nodeId);
    }
    CCNode::release();
  }
  // attribs
  void setUserObject(std::string const &id, CCObject *value);
  // dom
  void updateNodeDepth() {
    auto p = static_cast<CCNode2*>(m_pParent);
    if (p==nullptr) m_fields->depth = 0;
    else m_fields->depth = p->m_fields->depth+1;
    for (auto* c : geode::cocos::CCArrayExt<CCNode2*>(m_pChildren)) {
      c->updateNodeDepth();
    }
  }
  void setParent(CCNode* parent) {
    CCNode::setParent(parent);
    updateNodeDepth();
  }

  // child nodes
  void fireChildCountEvent() {
    if (DOMDomainDisabled) return;
    fireDOMEvent("childNodeCountUpdated", {
      {"nodeId",m_fields->nodeId}, 
      {"childNodeCount",getChildrenCount()}
    });
  }
  void addChild(CCNode *child);
  void removeChild(CCNode *child);
};

struct DOMNode {
  int nodeId;
  int parentId;
  int childNodeCount;
  matjson::Array children;
  matjson::Object attributes;

  /// for from_json
  DOMNode(
    int _nodeId,
    int _parentId,
    int _childNodeCount,
    matjson::Array _children,
    matjson::Object _attributes
  ): 
    nodeId(_nodeId), 
    parentId(_parentId), 
    childNodeCount(_childNodeCount), 
    children(_children),
    attributes(_attributes)
  {}

  DOMNode(CCNode *node, int depth = 1);
};

matjson::Value jsonValueOf(CCObject* obj);
template<>
struct matjson::Serialize<DOMNode> {
  static DOMNode from_json(const matjson::Value& value) {
    return DOMNode {
      value["nodeId"].as_int(),
      value["parentId"].as_int(),
      value["childNodeCount"].as_int(),
      value["children"].as_array(),
      value["attributes"].as_object()
    };
  }
  static matjson::Value to_json(const DOMNode& node) {
    return matjson::Object {
      {"nodeId", node.nodeId},
      {"parentId", node.parentId},
      {"childNodeCount", node.childNodeCount},
      {"children", node.children},
      {"attributes", node.attributes},
    };
  }
};

matjson::Value jsonValueOf(CCObject* obj) {
  if (obj == nullptr) return nullptr;
  auto j = geode::cast::typeinfo_cast<CCBool*>(obj);
  if (j != nullptr) {
    return j->getValue();
  }
#ifndef GEODE_IS_MACOS/
  else if (auto idk = geode::cast::typeinfo_cast<CCString*>(obj)) {
    return std::string(idk->getCString());
  }
  else if (auto dict = geode::cast::typeinfo_cast<CCDictionary*>(obj)) {
    matjson::Object d;
    CCDictElement* o;
    CCDICT_FOREACH(dict,o) {
      d.insert(std::make_pair(
        std::string(o->getStrKey()), 
        jsonValueOf(o->getObject())
      ));
    }
    return d;
  }
#endif
  else if (auto ni = geode::cast::typeinfo_cast<CCInteger*>(obj)) {
    return ni->getValue();
  }
  else if (auto m = geode::cast::typeinfo_cast<CCArray*>(obj)) {
    matjson::Array arr;
    arr.reserve(m->count());
    CCObject* o;
    CCARRAY_FOREACH(m, o) {
      arr.push_back(jsonValueOf(o));
    }
    return arr;
  }
  else if (auto n = geode::cast::typeinfo_cast<CCNode*>(obj)) {
    return DOMNode(n);
  }
  return "CCObject (unsupported)";
};

int nodeIdOf(CCNode* n) {
  return static_cast<CCNode2*>(n)->m_fields->nodeId;
}

DOMNode::DOMNode(CCNode *node, int depth)
  : nodeId(nodeIdOf(node)), parentId(nodeIdOf(node)), childNodeCount(node->getChildrenCount()) {
  children.reserve(childNodeCount);
  if (depth == 0) {
    for (auto *c : geode::cocos::CCArrayExt<CCNode *>(node->getChildren())) {
      children.push_back(DOMNode(c,depth-1));
    }
  }

  /*
  auto modify = static_cast<CCNode2*>(node);
  if (IsDebuggerPresent() && !DOMDomainDisabled) DebugBreak();
  auto fields = modify->m_fields;
  for (auto& pair : fields->m_userObjects) {
    attributes[pair.first] = jsonValueOf(pair.second);
  }
  */
}

void CCNode2::removeChild(CCNode *child) {
  CCNode::removeChild(child);
  //if (DOMDomainDisabled) return;
  if (true) return;
  fireDOMEvent("childNodeRemoved", {
    {"parentNodeId", nodeIdOf(this)}, 
    {"nodeId", nodeIdOf(child)}
  });
}

void CCNode2::addChild(CCNode *child) {
  auto p = child->getParent();
  CCNode::addChild(child);
  if (true) return;
  //if (DOMDomainDisabled) return;
  matjson::Object resp{
    {"parentNodeId", nodeIdOf(this)},
    {"node", DOMNode(child)}
  };
  if (p) {
    resp["previousNodeId"] = nodeIdOf(p);
  }
  fireDOMEvent("childNodeInserted", resp);
}

void CCNode2::setUserObject(std::string const &id, CCObject *value) {
  CCNode::setUserObject(id, value);
  if (value) {
    m_fields->m_userObjects[id] = value;
    if (DOMDomainDisabled) return;
    fireDOMEvent("attributeModified", {
      {"nodeId", nodeIdOf(this)},
      {"name", id},
      {"value", jsonValueOf(value)}
    });
  } else {
    m_fields->m_userObjects.erase(id);
    if (DOMDomainDisabled) return;
    fireDOMEvent("attributeRemoved", {
      {"nodeId", nodeIdOf(this)}, 
      {"name", id}
    });
  }
}


#include <Geode/modify/CCDirector.hpp>
struct directorhook : geode::Modify<directorhook, CCDirector> {
  bool replaceScene(CCScene* scene) {
    bool ret = CCDirector::replaceScene(scene);
    if (!geode::cast::typeinfo_cast<CCTransitionScene*>(scene)) {
      fireDOMEvent("documentUpdated");
    };
    return ret;
  }
  bool pushScene(CCScene* scene) {
    bool ret = CCDirector::pushScene(scene);
    if (!geode::cast::typeinfo_cast<CCTransitionScene*>(scene)) {
      fireDOMEvent("documentUpdated");
    }
    return ret;
  }
};

$domainMethod(describeNode) {
  CCNode* node = getNodeAt(as_optional_of<int>(params["nodeId"]).value_or(0)); 
  if (node) {
    int depth = 1;
    if (!params["depth"].is_null()) {
      depth = params["depth"].as_int();
    }
    return geode::Ok(matjson::Object{
      {"node",DOMNode(node,depth)}
    });
  } else {
    return geode::Err(
      std::make_tuple(-32602, "Node doesn't exist.")
    );
  }
}
$domainMethod(disableDOM) {
  DOMDomainDisabled = true;
  return geode::Ok(matjson::Object{});
}
$domainMethod(enableDOM) {
  DOMDomainDisabled = false;
  return geode::Ok(matjson::Object{});
}
$domainMethod(getAttributes) {
  CCNode* node = getNodeAt(as_optional_of<int>(params["nodeId"]).value_or(0)); 
  if (node) {
    matjson::Object attributes;
    auto modify = static_cast<CCNode2*>(node);
    auto fields = modify->m_fields;
    for (auto& pair : fields->m_userObjects) {
      attributes[pair.first] = jsonValueOf(pair.second);
    }
    return geode::Ok(matjson::Object{
      {"attributes",attributes}
    });
  } else {
    return geode::Err(
      std::make_tuple(-32602, "Node doesn't exist.")
    );
  }
}
$domainMethod(getAttribute) {
  CCNode* node = getNodeAt(as_optional_of<int>(params["nodeId"]).value_or(0)); 
  if (node) {
    return geode::Ok(matjson::Object{
      {"value", jsonValueOf(node->getUserObject(params["name"].as_string()))}
    });
  } else {
    return geode::Err(
      std::make_tuple(-32602, "Node doesn't exist.")
    );
  }
}
$domainMethod(getBoxModel) {
  CCNode* node = getNodeAt(as_optional_of<int>(params["nodeId"]).value_or(0)); 
  if (node) {
    return geode::Ok(matjson::Object{
      {"width", node->CCNode::getContentSize().width},
      {"height", node->CCNode::getContentSize().height}
    });
  } else {
    return geode::Err(
      std::make_tuple(-32602, "Node doesn't exist.")
    );
  }
}
$domainMethod(getDocument) {
  CCNode* node = CCDirector::sharedDirector()->getRunningScene(); 
  if (node) {
    DOMDomainDisabled = false;
    int depth = 1;
    if (!params["depth"].is_null()) {
      depth = params["depth"].as_int();
    }
    return geode::Ok(matjson::Object{
      {"root",DOMNode(node,depth)}
    });
  } else {
    return geode::Err(
      std::make_tuple(-32602, "No running scene.")
    );
  }
}
$domainMethod(getNodeForLocation) {
  if (DOMDomainDisabled) {
    return geode::Ok(matjson::Object{});
  }
  auto root = CCDirector::sharedDirector()->getRunningScene();
  if (root) {
    CCPoint point{
      (float)params["x"].as_int(),
      (float)params["y"].as_int()
    };
    int largest = 0;
    CCNode* ret = root;
    std::queue<geode::cocos::CCArrayExt<CCNode*>> queue;
    queue.push(root->getChildren());

    auto iiv = params["includeInvalidNode"];
    bool includeInvalid = iiv.is_null() ? true : iiv.as_bool();

    while (!queue.empty()) {
      auto children = queue.front();
      for (auto* c : children) {
        auto size = c->CCNode::getScaledContentSize();
        auto pos = c->CCNode::getPosition();
        if (
          pos.x <= point.x && pos.y <= point.y
          &&
          pos.x+size.width >= point.x && pos.y+size.height >= point.y
        ) {
          int depth = static_cast<CCNode2*>(c)->m_fields->depth;
          if (depth >= largest) {
            largest = depth;
            ret = c;
          }
          if (!includeInvalid) queue.push(c->getChildren());
        }
        if (includeInvalid) queue.push(c->getChildren());
      }
    }
    return geode::Ok(matjson::Object{
      {"nodeId", DOMNode(ret)}
    });
  } else {
    return geode::Err(
      std::make_tuple(-32602, "No running scene.")
    );
  }
}
$domainMethod(moveTo) {
  // parameters check
  int targetId = 0;
  if (true) {
    auto v = params["nodeId"];
    if (!v.is_number()) return errors::invalidParameter("Invalid 'nodeId' type.");
    targetId = v.as_int();
  }
  auto destId = 0;
  if (true) {
    auto v = params["targetNodeId"];
    if (!v.is_number()) return errors::invalidParameter("Invalid 'targetNodeId' type..");
    destId = v.as_int();
  }
  if (
    !params.contains("insertBeforeNodeId")
    &&
    !params.contains("insertAfterNodeId")
  ){
    return errors::invalidParameter("'insertBeforeNodeId' and 'insertAfterNodeId' cannot both be defined.");
  }
  auto beforeId = as_optional_of<int>(params["insertBeforeNodeId"]);
  auto afterId = as_optional_of<int>(params["insertAfterNodeId"]);

  // node check
  auto target = getNodeAt(targetId);
  auto dest = getNodeAt(destId);
  if (target == nullptr) {
    return errors::invalidParameter(fmt::format(
      "Node with identifier '{}' (as 'nodeId') not found.",
      targetId 
    ));
  }
  if (dest == nullptr) {
    return errors::invalidParameter(fmt::format(
      "Node with identifier '{}' (as 'targetNodeId') not found.",
      destId
    ));
  }
  if (!beforeId.has_value())
  dest->insertBefore(target, getNodeAt(beforeId.value()));
  else if (!afterId.has_value())
  dest->insertAfter(target, getNodeAt(afterId.value()));
  else dest->addChild(target);
  return geode::Ok(matjson::Object{});
}
$domainMethod(removeNode) { 
  CCNode* node = getNodeAt(as_optional_of<int>(params["nodeId"]).value_or(0)); 
  if (node) {
    geode::queueInMainThread([node]{
      node->removeFromParentAndCleanup(false);
    });
  }
  return geode::Ok(matjson::Object{});
}
$domainMethod(querySelector) { 
  CCNode* node = getNodeAt(as_optional_of<int>(params["nodeId"]).value_or(0)); 
  if (node) {
    if (auto ret = node->querySelector(params["selector"].as_string())) {
      return geode::Ok(matjson::Object{
        {"node", DOMNode(ret)}
      });  
    }
    return errors::invalidParameter("idk");
  }
  return errors::internalError("Node doesn't exist.");
}
CCObject* cocosObjOf(matjson::Value& val) {
  if (val.is_null()) return nullptr;
  else if (val.is_array()) {
    CCArray* arr = CCArray::create();
    for (auto& i : val.as_array()) {
      arr->addObject(cocosObjOf(i));
    }
    return arr;
  }
  else if (val.is_bool()) return CCBool::create(val.as_bool());
  else if (val.is_number()) return CCDouble::create(val.as_double());
  else if (val.is_object()) {
    CCDictionary* dict = CCDictionary::create();
    for (auto& p : val.as_object()) {
      dict->setObject(cocosObjOf(p.second), p.first);
    }
  }
  return nullptr;
}
$domainMethod(setAttribute) {
  CCNode* node = getNodeAt(as_optional_of<int>(params["nodeId"]).value_or(0)); 
  if (node) {
    node->setUserObject(
      params["name"].as_string(),
      cocosObjOf(params["value"])
    );
    return geode::Ok(matjson::Object{});
  } else {
    return geode::Err(
      std::make_tuple(-32602, "Node doesn't exist.")
    );
  }
}
$domainMethod(removeAttribute) {
  params["value"] = nullptr;
  return setAttribute(params);
}

$execute {
  auto p = Protocol::get();
  p->registerFunction("DOM.describeNode", &describeNode, {"nodeId"});
  p->registerFunction("DOM.disable", &disableDOM);
  p->registerFunction("DOM.enable", &enableDOM);
  p->registerFunction("DOM.getAttributes", &getAttributes, {"nodeId"});
  p->registerFunction("DOM.getAttribute", &getAttribute, {"nodeId", "name"});
  p->registerFunction("DOM.getBoxModel", &getBoxModel, {"nodeId"});
  p->registerFunction("DOM.getDocument", &getDocument);
  p->registerFunction("DOM.getNodeForLocation", &getNodeForLocation, {"nodeId", "x", "y"});
  p->registerFunction("DOM.moveTo", &moveTo, {"nodeId","targetNodeId"});
  p->registerFunction("DOM.removeNode", &removeNode, {"nodeId"});
  p->registerFunction("DOM.querySelector", &querySelector, {"nodeId","selector"});
  p->registerFunction("DOM.removeAttribute", &removeAttribute, {"nodeId","name"});
  p->registerFunction("DOM.setAttribute", &setAttribute, {"nodeId","name","value"});
}
