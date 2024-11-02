GD DevTools Protocol Domains Reference (INCOMPLETE)
---

Except from some derivations (such as `Browser` domain renamed to `Game`), this api is similar to the [Chrome DevTools Protocol](https://chromedevtools.github.io/devtools-protocol/) so if you're familiar with it then there shouldn't be any issues working with this :D

> **Note** that this reference ==only documents what's currently available in the mod itself==, so don't expect every features of CDP available (especially the early releases of the mod)
> 
> **Note 2** most of this docs will sound similar to the docs from the aforementioned protocol and it's intended. Don't ask anything about that.


## `DOM` Domain
its what you see, you made that super duper excellently!  
(thats the cocos2d node tree)  
(node ids is actually the address of the node, the more you know)  

### Methods

#### DOM.describeNode
Describes node given its id, does not require domain to be enabled. Does not start tracking any objects, can be used for automation.

| Parameters | |
|---|---|
| nodeId | **[NodeId](#domnodeid)**<br>The node identifier |
| depth<br>(optional) | **int**<br>The maximum depth at which children should be retrieved, defaults to 1. Use -1 for the entire subtree or provide an integer larger than 0. |

| Return Object | |
|---|---|
| node | **[Node](#domnode)**<br>Node description |

#### DOM.disable
Disables DOM agent.

#### DOM.enable
Enables DOM agent.

#### DOM.getAttributes
Returns attributes for the specified node.  

| Parameters | |
|---|---|
| nodeId | **[NodeId](#domnodeid)**<br>Id of the node to retrieve attributes for. |

| Return Object | |
|---|---|
| attributes | **object[string, [AttributeType](#domattributetype)]**<br>An interleaved array of node attribute names and values. |

#### DOM.getAttribute
Returns a specific attribute for the specified node.  

| Parameters | |
|---|---|
| nodeId | **[NodeId](#domnodeid)**<br>Id of the node to retrieve attributes for. |
| name | **string**<br>Name of the attribute. |

| Return Object | |
|---|---|
| value | **[AttributeType](#domattributetype)**<br>Value of the requested attribute. |

#### DOM.getBoxModel
Returns boxes for the given node.

| Parameters | |
|---|---|
| nodeId | **[NodeId](#domnodeid)**<br>Identifier of the node. |

| Return Object | |
|---|---|
| model | **[BoxModel](#domattributetype)**<br>Box model for the node. |

#### DOM.getDocument
Returns the root DOM node (which is the current scene) (and optionally the subtree) to the caller. Implicitly enables the DOM domain events for the current target.

| Parameters | |
|---|---|
| depth<br>(optional) | **integer**<br>The maximum depth at which children should be retrieved, defaults to 1. Use -1 for the entire subtree or provide an integer larger than 0. |

| Return Object | |
|---|---|
| node | **[Node](#domnode)**<br>Node description |

#### DOM.getNodeForLocation
Returns node id at given location. Depending on whether DOM domain is enabled, nodeId is either returned or not.

| Parameters | |
|---|---|
| x | **integer**<br>X coordinate. |
| y | **integer**<br>Y coordinate. |

| Return Object | |
|---|---|
| nodeId | **[NodeId](#domnodeid)**<br>Id of the node at given coordinates, only when enabled and requested document. |

### Types 
#### DOM.BoxModel
Box model.  
Type: **object**
**Properties**

| Name | Description |
|---|---|
| width | **integer**<br>Node width. |
| height | **integer**<br>Node height. |

#### DOM.AttributeType
The supported attribute type.  
Allowed Types: `string, int, bool, array, object, {nodeId: int}`  
TODO: figure out how to differentiate between Node and object

#### DOM.Node
DOM interaction is implemented in terms of mirror objects that represent the actual DOM nodes. DOMNode is a base node mirror type.  
Type: **object**

**Properties**

| Name | Description |
|---|---|
| nodeId | **[NodeId](#domnodeid)**<br>Node identifier that is passed into the rest of the DOM messages as the `nodeId`. Backend will only push node with given `id` once. It is aware of all requested nodes and will only fire DOM events for nodes known to the client. |
| parentId<br>(optional) |  The id of the parent node if any. |
| childNodeCount | Child count for this node. |
| children | Child nodes of this node when requested with children. |
| attributes | Attributes of this node in the form of an object `{name1: value1, name2: value2, ...}` |

#### DOM.NodeId
Unique DOM node identifier.  
In GDP's case, it's the address of the node.  
Type: **integer**

#### Game
