#include "../WS.hpp"

static matjson::Object domain(
  std::string name,
  matjson::Value desc,
  bool incomplete,
  matjson::Array types,
  matjson::Array commands,
  matjson::Array events
) {
  auto ret = matjson::Object{
    {"domain", name},
    {"incomplete", incomplete},
    {"types", types},
    {"commands",commands},
    {"events",events}
  };
  if (desc.is_string()) ret["description"] = desc;
  return ret;
}

static matjson::Object type(
  std::string name,
  matjson::Value desc,
  std::string type
) {
  return matjson::Object {
    {"id", name},
    {"description", desc},
    {"type", type}
  };
}

static matjson::Object enumtype(
  std::string name,
  matjson::Value desc,
  std::vector<std::string> enumVal
) {
  return matjson::Object {
    {"id", name},
    {"description", desc},
    {"type", "string"},
    {"enum",enumVal}
  };
}

static matjson::Object objtype(
  std::string name,
  matjson::Value desc,
  matjson::Array props
) {
  return matjson::Object {
    {"id", name},
    {"description", desc},
    {"type", "object"},
    {"properties", props}
  };
}

static matjson::Object objprop(
  std::string name,
  matjson::Value desc,
  std::string type,
  bool optional = false
) {
  return matjson::Object {
    {"id", name},
    {"description", desc},
    {"type", type},
    {"optional", optional}
  };
}

static matjson::Object objprop2(
  std::string name,
  matjson::Value desc,
  std::string ref,
  bool optional = false
) {
  return matjson::Object {
    {"id", name},
    {"description", desc},
    {"$ref", ref},
    {"optional", optional}
  };
}

static matjson::Object retVal = {
  {"domains", matjson::Array{
    domain(
      "DOM",
      "This domain exposes DOM read/write operations. Each DOM Node is represented with its mirror object that has an `id`. This `id` can be used to get additional information on the Node, and more. It is important that client receives DOM events only for the nodes that are known to the client. Backend keeps track of the nodes that were sent to the client and never sends the same node twice. It is client's responsibility to collect information about the nodes that were sent to the client.",
      false,
      {
        enumtype("AttributeType", "Attribute type.", {
          "string", "integer", "bool", "array", "object"
        }),
        objtype("BoxModel", "Box model.", {
          objprop("width", "Node width.", "integer"),
          objprop("height", "Node height.", "integer")
        }),
        objtype("Node", "DOM interaction is implemented in terms of mirror objects that represent the actual DOM nodes. DOMNode is a base node mirror type.", {
          objprop2("nodeId", "Node identifier that is passed into the rest of the DOM messages as the <code>nodeId</code>. Backend will only push node with given <code>id</code> once. It is aware of all requested nodes and will only fire DOM events for nodes known to the client.", "NodeId"),
          objprop2("parentNodeId", "The id of the parent node if any.", "NodeId", true),
          objprop("childNodeCount", "Child count for this node.", "integer")
        })
      },
      {}, {}
    )
  }}
};