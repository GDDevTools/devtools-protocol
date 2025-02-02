#include <Geode/utils/cocos.hpp>
#include "../WS.hpp"
#include "Runtime.hpp"
#include <matjson.hpp>
#include <Geode/Geode.hpp>
#include <queue>

using namespace cocos2d;
#include "DOM.hpp"

inline void fireDOMEvent(std::string eventName, matjson::Value const& content = {}) {
  fireEvent("DOM."+eventName, content);
};

static std::unordered_map<int, CCNode*> s_nodes;

CCNode* getNodeAt(int id) {
  if (id < 0) return nullptr;
  auto e = s_nodes.find(id);
  if (e!=s_nodes.end()) {
    return e->second;
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
    intmax_t nodeId = 0;
    bool initFinished = false;
  };
  /*
  void constructor() {
    cocos2d::CCNode();
  }
  */
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
  // assigns a node id when this node actually participates in the tree
  void setParent(CCNode* parent) {
    // since ctor hook randomly doesnt work
    if (!m_fields->initFinished) {
      // basically init
      m_fields->nodeId = ++highestNodeId;
    }
    s_nodes[m_fields->nodeId] = this;
    m_fields->initFinished = true;
    CCNode::setParent(parent);
    updateNodeDepth();
  }

  // child nodes
  void fireChildCountEvent() {
    return;
    fireDOMEvent("childNodeCountUpdated", matjson::makeObject({
      {"nodeId",m_fields->nodeId}, 
      {"childNodeCount",getChildrenCount()}
    }));
  }
  void addChild(CCNode *child);
  void removeChild(CCNode *child);
};

struct DOMNode {
  intmax_t nodeId;
  intmax_t parentId;
  // nope, not the numerical node id you're looking for
  std::string geodeNodeId;
  std::string type;
  int childNodeCount;
  // array
  matjson::Value children;
  // object
  matjson::Value attributes;

  /// for from_json
  DOMNode(
    intmax_t _nodeId,
    intmax_t _parentId,
    std::string _geodeNodeId,
    std::string _type,
    int _childNodeCount,
    decltype(children) _children,
    matjson::Value _attributes
  ): 
    nodeId(_nodeId), 
    parentId(_parentId), 
    geodeNodeId(_geodeNodeId), 
    type(_type),
    childNodeCount(_childNodeCount), 
    children(_children),
    attributes(_attributes)
  {}

  DOMNode(CCNode *node, int depth = 1);
};

matjson::Value jsonValueOf(CCObject* obj);
template<>
struct matjson::Serialize<DOMNode> {
  static geode::Result<DOMNode> fromJson(const matjson::Value& value) {
    return geode::Ok(DOMNode {
      value["nodeId"].asInt().unwrapOr(-1),
      value["parentId"].asInt().unwrapOr(-1),
      value["geodeNodeId"].asString().unwrapOr(""),
      value["nodeType"].asString().unwrapOr("node"),
      (int)value["childNodeCount"].asInt().unwrapOr(0),
      value["children"].asArray().unwrapOr(std::vector<matjson::Value>{}),
      value["attributes"]
    });
  }
  static matjson::Value toJson(const DOMNode& node) {
    auto b = matjson::makeObject({
      {"nodeId", node.nodeId},
      {"nodeType", node.type},
      {"childNodeCount", node.childNodeCount},
      {"children", node.children.asArray().unwrapOr(std::vector<matjson::Value>{})},
      {"attributes", node.attributes},
    });
    if (node.parentId >= 0) b.set("parentId", node.parentId);
    if (!node.geodeNodeId.empty()) b.set("geodeNodeId", node.geodeNodeId);
    return b;
  }
};

matjson::Value jsonValueOf(CCObject* obj) {
  if (obj == nullptr) return nullptr;
  auto j = geode::cast::typeinfo_cast<CCBool*>(obj);
  if (j != nullptr) {
    return j->getValue();
  }
#ifndef GEODE_IS_MACOS
  else if (auto idk = geode::cast::typeinfo_cast<CCString*>(obj)) {
    return std::string(idk->getCString());
  }
  else if (auto dict = geode::cast::typeinfo_cast<CCDictionary*>(obj)) {
    matjson::Value d;
    CCDictElement* o;
    CCDICT_FOREACH(dict,o) {
      d.set(
        std::string(o->getStrKey()), 
        jsonValueOf(o->getObject())
      );
    }
    return d;
  }
#endif
  else if (auto ni = geode::cast::typeinfo_cast<CCInteger*>(obj)) {
    return ni->getValue();
  }
  else if (auto m = geode::cast::typeinfo_cast<CCArray*>(obj)) {
    auto arr = matjson::Value::array();
    arr.asArray().unwrap().reserve(m->count());
    CCObject* o;
    CCARRAY_FOREACH(m, o) {
      arr.push(jsonValueOf(o));
    }
    return arr;
  }
  else if (auto n = geode::cast::typeinfo_cast<CCNode*>(obj)) {
    return DOMNode(n);
  }
  return "CCObject (unsupported)";
};

int nodeIdOf(CCNode* n) {
  if (n==nullptr) return -1; // no parent; DOMNode will omit this value
                             // (the parentNodeId property but i will make an assert later)
  return static_cast<CCNode2*>(n)->m_fields->nodeId;
}

// the depth is used as the children inclusion depth so do NOT use it as the depth property
DOMNode::DOMNode(CCNode *node, int depth)
  : nodeId(nodeIdOf(node)), parentId(nodeIdOf(node->m_pParent)), geodeNodeId(node->getID()), childNodeCount(node->getChildrenCount()), children(matjson::Value::array()) {
#pragma region DOMNode children
  //children.reserve(childNodeCount);
  if (depth != 0) {
    for (auto *c : geode::cocos::CCArrayExt<CCNode *>(node->getChildren())) {
      children.push(DOMNode(c,depth-1));
    }
  }
#pragma endregion
#pragma region DOMNode nodeType
  // my mod do this
  if (auto typeOverride = static_cast<CCString*>(node->getUserObject("puppeteer/nodeTypeOverride"))) {
    type = typeOverride->getCString();
  }
  // for now its only this much
  else if (dynamic_cast<CCMenuItem*>(node)) {
    type = "button";
  }
  else if (dynamic_cast<CCScene*>(node)) {
    type = "scene";
  } 
  else {
    type = "node";
  }
#pragma endregion

  /*
  auto modify = static_cast<CCNode2*>(node);
  auto fields = modify->m_fields;
  for (auto& pair : fields->m_userObjects) {
    attributes[pair.first] = jsonValueOf(pair.second);
  }
  */
}

void CCNode2::removeChild(CCNode *child) {
  fireDOMEvent("childNodeRemoved", matjson::makeObject({
    {"parentNodeId", nodeIdOf(this)}, 
    {"nodeId", nodeIdOf(child)}
  }));
  fireDOMEvent("childNodeCountUpdated", matjson::makeObject({
    {"nodeId", nodeIdOf(child)},
    {"childNodeCount", getChildrenCount()}
  }));
  CCNode::removeChild(child);
}

void CCNode2::addChild(CCNode *child) {
  auto p = child->getParent();
  CCNode::addChild(child);
  //return;
  auto resp = matjson::makeObject({
    {"parentNodeId", nodeIdOf(this)},
    {"node", DOMNode(child)}
  });
  if (p) {
    resp["previousNodeId"] = nodeIdOf(p);
  }
  fireDOMEvent("childNodeInserted", resp);
  
  fireDOMEvent("childNodeCountUpdated", matjson::makeObject({
    {"nodeId", nodeIdOf(child)},
    {"childNodeCount", getChildrenCount()}
  }));
}

void CCNode2::setUserObject(std::string const &id, CCObject *value) {
  CCNode::setUserObject(id, value);
  if (value) {
    m_fields->m_userObjects[id] = value;
    return;
    fireDOMEvent("attributeModified", matjson::makeObject({
      {"nodeId", nodeIdOf(this)},
      {"name", id},
      {"value", jsonValueOf(value)}
    }));
  } else {
    m_fields->m_userObjects.erase(id);
    return;
    fireDOMEvent("attributeRemoved", matjson::makeObject({
      {"nodeId", nodeIdOf(this)}, 
      {"name", id}
    }));
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
$domainMethod(click) {
  CCNode* node = getNodeAt((params["nodeId"]).asInt().unwrapOr(0)); 
  if (node) {
    if (auto b = dynamic_cast<CCMenuItem*>(node)) {
      b->activate();
    } else if (
      std::string(static_cast<CCString*>(
        b->getUserObject("puppeteer/nodeTypeOverride")
      )->getCString()) == "button"
    ){
      geode::DispatchEvent("puppeteer/nodeClicked", node).post();
    } else {
      return errors::invalidParameter("Node isn't classified as a button.");
    }
    return emptyResponse();
  }
  return errors::internalError("Node doesn't exist.");
}
$domainMethod(describeNode) {
  CCNode* node = getNodeAt(params["nodeId"].asInt().unwrapOr(0)); 
  if (node) {
    int depth = params["depth"].asInt().unwrapOr(1);
    return geode::Ok(matjson::makeObject({
      {"node",DOMNode(node,depth)}
    }));
  } else {
    return errors::invalidParameter("Node doesn't exist.");
  }
}
$domainMethod(disableDOM) {
  info.setDomainEnabled("DOM", false);
  return emptyResponse();
}
$domainMethod(enableDOM) {
  info.setDomainEnabled("DOM", true);
  return emptyResponse();
}
$domainMethod(getAttributes) {
  CCNode* node = getNodeAt(params["nodeId"].asInt().unwrapOr(0)); 
  if (node) {
    matjson::Value attributes;
    auto modify = static_cast<CCNode2*>(node);
    auto fields = modify->m_fields;
    for (auto& pair : fields->m_userObjects) {
      attributes[pair.first] = jsonValueOf(pair.second);
    }
    return geode::Ok(matjson::makeObject({
      {"attributes",attributes}
    }));
  } else {
    return errors::invalidParameter("Node doesn't exist.");
  }
}
$domainMethod(getAttribute) {
  CCNode* node = getNodeAt(params["nodeId"].asInt().unwrapOr(0)); 
  if (node) {
    return geode::Ok(matjson::makeObject({
      {"value", jsonValueOf(node->getUserObject(params["name"].asString().unwrap()))}
    }));
  } else {
    return errors::invalidParameter("Node doesn't exist.");
  }
}
$domainMethod(getBoxModel) {
  CCNode* node = getNodeAt(params["nodeId"].asInt().unwrapOr(0)); 
  if (node) {
    return geode::Ok(matjson::makeObject({
      {"width", node->CCNode::getContentSize().width},
      {"height", node->CCNode::getContentSize().height}
    }));
  } else {
    return errors::invalidParameter("Node doesn't exist.");
  }
}
$domainMethod(getDocument) {
  CCNode* node = CCDirector::sharedDirector()->getRunningScene(); 
  if (node) {
    int depth = params["depth"].asInt().unwrapOr(1);
    return geode::Ok(matjson::makeObject({
      {"root",DOMNode(node,depth)}
    }));
  } else {
    return errors::invalidParameter("No running scene.");
  }
}
$domainMethod(getNodeForLocation) {
  if (info.isDomainEnabled("DOM")) {
    return emptyResponse();
  }
  auto root = CCDirector::sharedDirector()->getRunningScene();
  if (root) {
    CCPoint point{
      (float)params["x"].asInt().unwrap(),
      (float)params["y"].asInt().unwrap()
    };
    int largest = 0;
    CCNode* ret = root;
    std::queue<geode::cocos::CCArrayExt<CCNode*>> queue;
    queue.push(root->getChildren());

    auto iiv = params["includeInvalidNode"];
    bool includeInvalid = iiv.asBool().unwrapOr(true);

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
    return geode::Ok(matjson::makeObject({
      {"nodeId", DOMNode(ret)}
    }));
  } else {
    return errors::invalidParameter("No running scene.");
  }
}
$domainMethod(moveTo) {
  // parameters check
  int targetId = 0;
  if (true) {
    auto v = params["nodeId"];
    if (!v.isNumber()) return errors::invalidParameter("Invalid 'nodeId' type.");
    targetId = v.asInt().unwrap();
  }
  auto destId = 0;
  if (true) {
    auto v = params["targetNodeId"];
    if (!v.isNumber()) return errors::invalidParameter("Invalid 'targetNodeId' type..");
    destId = v.asInt().unwrap();
  }
  if (
    !params.contains("insertBeforeNodeId")
    &&
    !params.contains("insertAfterNodeId")
  ){
    return errors::invalidParameter("'insertBeforeNodeId' and 'insertAfterNodeId' cannot both be defined.");
  }
  auto beforeId = params["insertBeforeNodeId"].asInt();
  auto afterId = params["insertAfterNodeId"].asInt();

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
  if (beforeId.isErr())
  dest->insertBefore(target, getNodeAt(beforeId.unwrap()));
  else if (afterId.isErr())
  dest->insertAfter(target, getNodeAt(afterId.unwrap()));
  else dest->addChild(target);
  return emptyResponse();
}
$domainMethod(querySelector) { 
  CCNode* node = getNodeAt(params["nodeId"].asInt().unwrapOr(0)); 
  if (node) {
    if (auto ret = node->querySelector(params["selector"].asString().unwrap())) {
      return geode::Ok(matjson::makeObject({
        {"node", DOMNode(ret)}
      }));  
    }
    return errors::invalidParameter("idk");
  }
  return errors::internalError("Node doesn't exist.");
}
$domainMethod(removeNode) { 
  CCNode* node = getNodeAt(params["nodeId"].asInt().unwrapOr(0)); 
  if (node) {
    geode::queueInMainThread([node]{
      node->removeFromParent();
    });
  }
  return emptyResponse();
}

#include "jsenv/state.hpp"
#include "jsenv/dom/node.hpp"
$domainAsyncMethod(requestNode) {
  std::string objectId = params["objectId"].asString().unwrapOr("");
  if (objectId.empty()) return finish(errors::invalidParameter("No, you won't get anything from that kind of id."));

  geode::queueInMainThread([objectId, finish]{
    CScriptVar* o = getState()->first;
    while (o) {
      if (auto node = dynamic_cast<dom::CScriptVarNode*>(o)) {
        finish(geode::Ok(matjson::makeObject({
          {"nodeId", nodeIdOf(node->getNode())}
        })));
      }
      o = o->next;
    }
    return finish(errors::invalidParameter("No node with the specified ID was found."));
  });
}
$domainMethod(resolveNode) {
  auto nodeIdRes = params["nodeId"].asInt();
  if (nodeIdRes.isOk()) {
    int nodeId = nodeIdRes.unwrap();

    auto node = getNodeAt(nodeId);
    
    if (node) return errors::invalidParameter("No node with the specified ID was found.");

    return geode::Ok(matjson::makeObject({
      {"object", serializeRemoteObject(getAssociatedJSObject(node))}
    }));
  } else {
    return errors::invalidParameter("What are you looking for?");
  }
}

CCObject* cocosObjOf(matjson::Value& val) {
  if (val.isNull()) return nullptr;
  else if (val.isArray()) {
    CCArray* arr = CCArray::create();
    for (auto& i : val.asArray().unwrap()) {
      arr->addObject(cocosObjOf(i));
    }
    return arr;
  }
  else if (val.isBool()) return CCBool::create(val.asBool().unwrap());
  else if (val.isNumber()) return CCDouble::create(val.asDouble().unwrap());
  else{
    CCDictionary* dict = CCDictionary::create();
    for (auto& [k,v] : val) {
      dict->setObject(cocosObjOf(v), k);
    }
    return dict;
  }
}
$domainMethod(setAttribute) {
  CCNode* node = getNodeAt(params["nodeId"].asInt().unwrapOr(0)); 
  if (node) {
    node->setUserObject(
      params["name"].asString().unwrap(),
      cocosObjOf(params["value"])
    );
    return emptyResponse();
  } else {
    return errors::invalidParameter("Node doesn't exist.");
  }
}
$domainMethod(removeAttribute) {
  params["value"] = nullptr;
  return setAttribute(info, params);
}

$execute {
  auto p = Protocol::get();
  p->registerFunction("DOM.click", &click, {"nodeId"});
  p->registerFunction("DOM.describeNode", &describeNode, {"nodeId"});
  p->registerFunction("DOM.disable", &disableDOM);
  p->registerFunction("DOM.enable", &enableDOM);
  p->registerFunction("DOM.getAttributes", &getAttributes, {"nodeId"});
  p->registerFunction("DOM.getAttribute", &getAttribute, {"nodeId", "name"});
  p->registerFunction("DOM.getBoxModel", &getBoxModel, {"nodeId"});
  p->registerFunction("DOM.getDocument", &getDocument);
  p->registerFunction("DOM.getNodeForLocation", &getNodeForLocation, {"nodeId", "x", "y"});
  p->registerFunction("DOM.moveTo", &moveTo, {"nodeId","targetNodeId"});
  p->registerFunction("DOM.querySelector", &querySelector, {"nodeId","selector"});
  p->registerFunction("DOM.removeNode", &removeNode, {"nodeId"});
  p->registerFunction("DOM.requestNode", &requestNode, {"objectId"});
  p->registerFunction("DOM.resolveNode", &resolveNode, {"nodeId"});
  p->registerFunction("DOM.removeAttribute", &removeAttribute, {"nodeId","name"});
  p->registerFunction("DOM.setAttribute", &setAttribute, {"nodeId","name","value"});
}
